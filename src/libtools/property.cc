/*
 * Copyright 2009-2020 The VOTCA Development Team (http://www.votca.org)
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */

// Standard includes
#include <cstring>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <stack>
#include <stdexcept>
#include <string>

// Third party includes
#include <boost/algorithm/string.hpp>
#include <boost/format.hpp>
#include <expat.h>
#include <unistd.h>

// Local VOTCA includes
#include "votca/tools/colors.h"
#include "votca/tools/property.h"
#include "votca/tools/propertyiomanipulator.h"
#include "votca/tools/tokenizer.h"

namespace votca {
namespace tools {

// ostream modifier defines the output format, level, indentation
const Index Property::IOindex = std::ios_base::xalloc();

Property &Property::set(const std::string &key, const std::string &value) {
  Property &p = get(key);
  p.value() = value;
  return p;
}

Property &Property::add(const std::string &key, const std::string &value) {
  std::string path = _path;
  if (path != "") {
    path = path + ".";
  }
  _properties.push_back(Property(key, value, path + _name));
  _map[key] = Index(_properties.size()) - 1;
  return _properties.back();
}

bool Property::exists(const std::string &key) const {
  try {
    get(key);
  } catch (std::exception &) {
    return false;
  }
  return true;
}

const Property &Property::get(const std::string &key) const {
  Tokenizer tok(key, ".");
  Tokenizer::iterator n = tok.begin();
  if (n == tok.end()) {
    return *this;
  }

  const Property *p;
  std::map<std::string, Index>::const_iterator iter;
  if (*n == "") {
    p = this;
  } else {
    iter = _map.find(*n);
    if (iter == _map.end()) {
      throw std::runtime_error("property not found: " + key);
    }
    p = &_properties[((*iter).second)];
  }
  ++n;
  try {
    for (; n != tok.end(); ++n) {
      p = &p->get(*n);
    }
  } catch (std::runtime_error &) {  // catch here to get full key in
                                    // exception
    throw std::runtime_error("property not found: " + key);
  }

  return *p;
}

Property &Property::get(const std::string &key) {
  return const_cast<Property &>(static_cast<const Property &>(*this).get(key));
}

Property &Property::getOradd(const std::string &key) {
  if (exists(key)) {
    return get(key);
  } else {
    return add(key, "");
  }
}

std::vector<const Property *> Property::Select(
    const std::string &filter) const {
  Tokenizer tok(filter, ".");
  std::vector<const Property *> selection;
  if (tok.begin() == tok.end()) {
    return selection;
  }
  selection.push_back(this);
  for (const auto &n : tok) {
    std::vector<const Property *> children;
    for (const Property *p : selection) {
      for (const Property &s : p->_properties) {
        if (wildcmp(n, s.name())) {
          children.push_back(&s);
        }
      }
    }
    selection = children;
  }
  return selection;
}

std::vector<Property *> Property::Select(const std::string &filter) {
  Tokenizer tok(filter, ".");
  std::vector<Property *> selection;
  if (tok.begin() == tok.end()) {
    return selection;
  }
  selection.push_back(this);
  for (const auto &n : tok) {
    std::vector<Property *> children;
    for (Property *p : selection) {
      for (Property &s : p->_properties) {
        if (wildcmp(n, s.name())) {
          children.push_back(&s);
        }
      }
    }
    selection = children;
  }
  return selection;
}

static void start_hndl(void *data, const char *el, const char **attr) {
  std::stack<Property *> *property_stack =
      (std::stack<Property *> *)XML_GetUserData((XML_Parser *)data);

  Property *cur = property_stack->top();
  Property &np = cur->add(el, "");

  for (Index i = 0; attr[i]; i += 2) {
    np.setAttribute(attr[i], attr[i + 1]);
  }

  property_stack->push(&np);
}

static void end_hndl(void *data, const char *) {
  std::stack<Property *> *property_stack =
      (std::stack<Property *> *)XML_GetUserData((XML_Parser *)data);
  property_stack->pop();
}

void char_hndl(void *data, const char *txt, int txtlen) {
  std::stack<Property *> *property_stack =
      (std::stack<Property *> *)XML_GetUserData((XML_Parser *)data);

  Property *cur = property_stack->top();
  cur->value().append(txt, txtlen);
}

void Property::LoadFromXML(const std::string &filename) {
  std::ifstream fl;
  fl.open(filename);
  if (!fl.is_open()) {
    throw std::ios_base::failure("Error on open xml file: " + filename);
  }

  XML_Parser parser = XML_ParserCreate(nullptr);
  if (!parser) {
    throw std::runtime_error("Couldn't allocate memory for xml parser");
  }

  XML_UseParserAsHandlerArg(parser);
  XML_SetElementHandler(parser, start_hndl, end_hndl);
  XML_SetCharacterDataHandler(parser, char_hndl);

  std::stack<Property *> pstack;
  pstack.push(this);

  XML_SetUserData(parser, (void *)&pstack);
  while (!fl.eof()) {
    std::string line;
    getline(fl, line);
    line = line + "\n";
    if (line.length() > (size_t)std::numeric_limits<int>::max()) {
      throw std::runtime_error("Property::LoadFromXML: line is too long");
    }
    if (!XML_Parse(parser, line.c_str(), (int)line.length(), fl.eof())) {
      throw std::ios_base::failure(
          filename + ": Parse error at line " +
          boost::lexical_cast<std::string>(XML_GetCurrentLineNumber(parser)) +
          "\n" + XML_ErrorString(XML_GetErrorCode(parser)));
    }
  }
  fl.close();
  XML_ParserFree(parser);
}

void PrintNodeTXT(std::ostream &out, const Property &p, const Index start_level,
                  Index level = 0, std::string prefix = "",
                  std::string offset = "") {
  if ((p.value() != "") || p.HasChildren()) {
    if (level >= start_level) {
      if ((p.value()).find_first_not_of("\t\n ") != std::string::npos) {
        out << offset << prefix << " = " << p.value() << std::endl;
      }
    } else {
      prefix = "";
    }
  }

  for (const Property &prop : p) {
    level++;
    if (prefix == "") {
      PrintNodeTXT(out, prop, start_level, level, prefix + prop.name(), offset);
    } else {
      PrintNodeTXT(out, prop, start_level, level, prefix + "." + prop.name(),
                   offset);
    }
    level--;
  }
}

void PrintNodeXML(std::ostream &out, const Property &p,
                  PropertyIOManipulator *piom, Index level = 0,
                  std::string offset = "") {
  Property::const_AttributeIterator ia;
  bool linebreak = true;
  bool has_value = false;

  const ColorSchemeBase *color = &DEFAULT_COLORS;

  std::string indent("");
  Index start_level(0);

  if (piom) {
    start_level = piom->getLevel();
    indent = piom->getIndentation();
    color = piom->getColorScheme();
  }

  std::string cKey = color->Magenta();
  std::string cAttribute = color->Blue();
  std::string cAttributeValue = color->Green();
  std::string cReset = color->Reset();

  // print starting only from the start_level (the first node (level 0) can be
  // <> </>)
  if (level >= start_level) {
    // print the node name
    out << indent << offset << "<" << cKey << p.name() << cReset;
    // print the node attributes
    for (ia = p.firstAttribute(); ia != p.lastAttribute(); ++ia) {
      out << " " << cAttribute << ia->first << cReset << "=\""
          << cAttributeValue << ia->second << cReset << "\"";
    }
    // print node value if it is not empty
    has_value = ((p.value()).find_first_not_of("\t\n ") != std::string::npos);
    if (has_value || p.HasChildren()) {
      out << ">";
    } else {
      out << "/>" << std::endl;
    }
    if (has_value) {
      out << cAttributeValue << p.value() << cReset;
      linebreak = false;
    }

    // check if we need the end of the line or not
    if (!has_value && p.HasChildren()) {
      out << std::endl;
    }
    if (!has_value && !p.HasChildren()) {
      linebreak = false;
    }
  }

  // continue iteratively through the rest of the nodes
  for (const Property &prop : p) {
    level++;
    if (level > start_level) {
      offset += "\t";
    }
    PrintNodeXML(out, prop, piom, level, offset);
    if (level > start_level) {
      offset.resize(offset.size() - 1);
    }
    level--;
  }

  if (level >= start_level) {
    if (linebreak) {
      out << indent << offset << "</" << cKey << p.name() << cReset << ">"
          << std::endl;
    } else if (has_value) {
      out << "</" << cKey << p.name() << cReset << ">" << std::endl;
    }
  }
}

void PrintNodeTEX(std::ostream &out, const Property &p,
                  PropertyIOManipulator *piom, Index level = 0,
                  std::string prefix = "") {

  Index start_level = 0;
  if (piom) {
    start_level = piom->getLevel();
  }

  std::string head_name;
  std::string section("");  // reference of the description section in the
                            // manual
  std::string help("");
  // if this is the head node, print the header
  if (level == start_level) {

    std::string header_format(
        "\\subsection{%1%}\n"
        "\\label{%2%}\n%3%\n"
        "\\rowcolors{1}{invisiblegray}{white}\n"
        "{\\small\n "
        "\\begin{longtable}{m{3cm}|m{2cm}|m{1cm}|m{8cm}}\n"
        " option & default & unit & description\\\\\n\\hline\n");

    head_name = p.name();
    std::string label =
        "calc:" + head_name;  // reference of the xml file in the manual
    if (p.hasAttribute("section")) {
      section = p.getAttribute<std::string>("section");
    }
    if (p.hasAttribute("help")) {
      help = p.getAttribute<std::string>("help");
    }
    out << boost::format(header_format) % head_name % label % help;
    prefix = p.name();
  }

  if (level > start_level) {
    // if this node has children or a value or is not the first, start recursive
    // printing
    if ((p.value() != "" || p.HasChildren()) && level > -1) {
      std::string tex_name = boost::replace_all_copy(p.name(), "_", "\\_");
      std::string defaults("");  // default value if supplied
      if (p.hasAttribute("default")) {
        defaults = p.getAttribute<std::string>("default");
      }
      std::string unit("");  // unit, if supplied
      if (p.hasAttribute("unit")) {
        unit = p.getAttribute<std::string>("unit");
      }
      if (p.hasAttribute("help")) {
        help = p.getAttribute<std::string>("help");
      }

      std::string body_format(
          " \\hspace{%1%pt}\\hypertarget{%2%}{%3%} & %4% & %5% & %6% \\\\\n");

      out << boost::format(body_format) %
                 Index((level - start_level - 1) * 10) % prefix % tex_name %
                 defaults % unit % help;
    }
  }

  // continue iteratively through the rest of the nodes
  for (const Property &pp : p) {
    level++;
    if (prefix == "") {
      PrintNodeTEX(out, pp, piom, level, prefix);
    } else {
      PrintNodeTEX(out, pp, piom, level, prefix);
    }
    level--;
  }

  // if this is the head node, print the footer
  if (level == start_level) {
    std::string footer_format(
        "\\end{longtable}\n}\n"
        "\\noindent Return to the description of "
        "\\slink{%1%}{\\texttt{%2%}}.\n");

    out << boost::format(footer_format) % section % head_name;
  }
}

void PrintNodeHLP(std::ostream &out, const Property &p,
                  const Index start_level = 0, Index level = 0,
                  const std::string &prefix = "",
                  const std::string &offset = "") {

  using ColorRGB = Color<csRGB>;  // use the RGB palette
  ColorRGB RGB;                   // Instance of an RGB palette
  std::string fmt = "t|%1%%|15t|" + std::string(RGB.Blue()) + "%2%" +
                    std::string(RGB.Green()) + "%|40t|%3%%|55t|" +
                    std::string(RGB.Reset()) + "%4%\n";

  Index leveloffset = level;
  std::string help("");
  // if this is the head node, print the header
  if (level == start_level) {
    std::string head_name = std::string(RGB.Magenta()) + p.name();
    if (p.hasAttribute("help")) {
      if (p.hasAttribute("help")) {
        help = std::string(RGB.Red()) + p.getAttribute<std::string>("help");
      }
      out << boost::format(" %1%: %|18t| %2%" + std::string(RGB.Reset()) +
                           "\n") %
                 head_name % help;
    }
    leveloffset = 0;
    out << boost::format("%|3" + fmt) % "OPTION" % "DEFAULT" % "UNIT" %
               "DESCRIPTION";
  }

  if (level > start_level) {
    std::string ofmt =
        "%|" + boost::lexical_cast<std::string>(leveloffset) + fmt;
    std::string unit("");
    if (p.hasAttribute("unit")) {
      unit = p.getAttribute<std::string>("unit");
    }
    std::string defaults("");
    if (p.hasAttribute("default")) {
      defaults = p.getAttribute<std::string>("default");
    }
    if (p.hasAttribute("help")) {
      help = p.getAttribute<std::string>("help");
    }
    if (!unit.empty()) {
      unit = "[" + unit + "]";
    }
    if (!defaults.empty()) {
      defaults = "(" + defaults + ")";
    }

    std::string name = p.name();

    out << boost::format(ofmt) % name % defaults % unit % help;
  }

  for (const Property &pp : p) {
    level++;
    if (prefix == "") {
      PrintNodeHLP(out, pp, start_level, level, pp.name(), offset);
    } else {
      PrintNodeHLP(out, pp, start_level, level, prefix + "." + pp.name(),
                   offset);
    }
    level--;
  }
}

std::ostream &operator<<(std::ostream &out, const Property &p) {
  if (!out.good()) {
    return out;
  }

  std::ostream::sentry sentry(out);

  if (sentry) {
    // get the property format object attached to the stream
    PropertyIOManipulator *pm =
        (PropertyIOManipulator *)out.pword(int(Property::getIOindex()));

    std::string indentation("");
    Index level = 0;

    PropertyIOManipulator::Type type = PropertyIOManipulator::XML;
    if (pm) {
      indentation = pm->getIndentation();
      level = pm->getLevel();
      type = pm->getType();
      // check if we > or >> to a file and remove color codes
      // if ( out.tellp() != -1 )  - not suitable for pipes
      if (!isatty(STDOUT_FILENO) || !isatty(STDERR_FILENO)) {
        pm->setColorScheme<csDefault>();
      }
    }

    switch (type) {
      default:
        PrintNodeTXT(out, p, level);
        break;
      case PropertyIOManipulator::XML:
        PrintNodeXML(out, p, pm);
        break;
      case PropertyIOManipulator::TXT:
        PrintNodeTXT(out, p, level, 0, "", indentation);
        break;
      case PropertyIOManipulator::TEX:
        PrintNodeTEX(out, p, pm);
        break;
      case PropertyIOManipulator::HLP:
        PrintNodeHLP(out, p, level, 0, "", indentation);
        break;
    }
  }

  return out;
}
}  // namespace tools
}  // namespace votca
