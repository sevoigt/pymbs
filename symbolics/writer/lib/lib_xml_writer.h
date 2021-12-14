/*
    Copyright (C) 2010, Ferruccio Barletta (ferruccio.barletta@gmail.com)

    Permission is hereby granted, free of charge, to any person
    obtaining a copy of this software and associated documentation
    files (the "Software"), to deal in the Software without
    restriction, including without limitation the rights to use,
    copy, modify, merge, publish, distribute, sublicense, and/or sell
    copies of the Software, and to permit persons to whom the
    Software is furnished to do so, subject to the following
    conditions:

    The above copyright notice and this permission notice shall be
    included in all copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
    EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
    OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
    NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
    HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
    WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
    FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
    OTHER DEALINGS IN THE SOFTWARE.
*/

#ifndef LIB_XML_WRITER_HPP
#define LIB_XML_WRITER_HPP

#include <string>
#include <iostream>
#include <sstream>
#include <stack>
#include <cassert>

namespace xml {

    class element;

    //
    //	xml::writer class
    //
    class writer
    {
    public:
        // writer must be bound to an ostream
        writer(std::ostream& os) : os(os), need_header(true), indentLevel(0) {}
        ~writer(void) { assert(elements.empty()); }

    private:
        std::ostream& os;               // output stream
        bool need_header;               // have we written an XML header yet?
        std::stack<element*> elements;  // stack of open element tags
		int indentLevel;				// the current Level of indentation

        // write XML header, if necessary
        writer& header() {
            if (need_header) {
                os << "<?xml version=\"1.0\" encoding=\"utf-8\"?>";
                need_header = false;
            }
            return *this;
        }

		// write indentation whitespaces
		writer& indent() {
			putc('\n');
			for (int i = 0; i < indentLevel; ++i)
				puts("  ");
			return *this;
		}

        // write a single character to the output stream
        writer& putc(char c) {
            os.put(c);
            return *this;
        }

        // write a string to the output stream
        writer& puts(const char* str) {
            os << str;
            return *this;
        }

        friend class element;
    };

    //
    //	xml::element class
    //
    class element
    {
    public:
        // create a new element tag, bound to an xml::writer
        element(const char* name, writer& wr, bool attrLinebreaks=true) : name(name), wr(wr), attrLB(attrLinebreaks) {
            assert(name != 0);
            check_parent();
            wr.header().indent().putc('<').puts(name);
            tagopen = true;
			wr.indentLevel++;
            wr.elements.push(this);
        }

        // close the current element tag
        ~element() {
            if (!wr.elements.empty() && wr.elements.top() == this) {
                wr.elements.pop();
				wr.indentLevel--;
                if (tagopen)
                    wr.puts("/>");
                else
                    wr.indent().puts("</").puts(name).putc('>');
            }
        }

        // write an attribute for the current element
        element& attr(const char* name, const char* value) {
            assert(name != 0);
            assert(value != 0);
            assert(tagopen);
            if (attrLB) 
				wr.indent();
			else
				wr.putc(' ');
			wr.puts(name).puts("=\"");
            qputs(value);
            wr.putc('"');
            return *this;
        }

        // attr() overload for std::string type
        element& attr(const char* name, const std::string& value) { return attr(name, value.c_str()); }

        // attr() function template for all streamable types
        template <class T>
        element& attr(const char* name, T value) {
            std::stringstream ss;
            ss << value;
            attr(name, ss.str());
            return *this;
        }

        // write text content for the current element
        element& contents(const char* str) {
            assert(str != 0);
            check_parent();
			wr.indent().putc(' ');
            qputs(str);
            return *this;
        }

        // contents() overload for std::string type
        element& contents(const std::string& str) { return contents(str.c_str()); }

        // contents() function template for all streamable types
        template <class T>
        element& contents(T value) {
            std::stringstream ss;
            ss << value;
            contents(ss.str());
            return *this;
        }

    private:
        writer& wr;         // bound XML writer
        const char* name;   // name of current element
        bool tagopen;       // is the element tag for this element still open?
		bool attrLB;		// shall there be linebreaks between attributes?

        // write a string quoting characters which have meaning in xml
        element& qputs(const char* str) {
            for (; *str; ++str)
                switch (*str) {
                    case '&': wr.puts("&amp;"); break;
                    case '<': wr.puts("&lt;"); break;
                    case '>': wr.puts("&gt;"); break;
                    case '\'': wr.puts("&apos;"); break;
                    case '"': wr.puts("&quot;"); break;
                    default: wr.putc(*str); break;
                }
            return *this;
        }

        // check to see if we have a parent tag which needs to be closed
        void check_parent() {
            if (!wr.elements.empty() && wr.elements.top()->tagopen) {
                wr.putc('>');
                wr.elements.top()->tagopen = false;
            }
        }
    };

}

#endif
