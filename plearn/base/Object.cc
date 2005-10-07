// -*- C++ -*-

// PLearn (A C++ Machine Learning Library)
// Copyright (C) 1998 Pascal Vincent
// Copyright (C) 1999-2002 Pascal Vincent, Yoshua Bengio and University of Montreal
// Copyright (C) 2002 Frederic Morin

// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
// 
//  1. Redistributions of source code must retain the above copyright
//     notice, this list of conditions and the following disclaimer.
// 
//  2. Redistributions in binary form must reproduce the above copyright
//     notice, this list of conditions and the following disclaimer in the
//     documentation and/or other materials provided with the distribution.
// 
//  3. The name of the authors may not be used to endorse or promote
//     products derived from this software without specific prior written
//     permission.
// 
// THIS SOFTWARE IS PROVIDED BY THE AUTHORS ``AS IS'' AND ANY EXPRESS OR
// IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
// OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN
// NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
// SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED
// TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
// PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
// LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
// NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
// SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
// 
// This file is part of the PLearn library. For more information on the PLearn
// library, go to the PLearn Web site at www.plearn.org


/* *******************************************************      
 * $Id$
 * AUTHORS: Pascal Vincent & Yoshua Bengio
 * This file is part of the PLearn library.
 ******************************************************* */

#include "Object.h"
#include "stringutils.h"    //!< For removeblanks.
#include <plearn/io/fileutils.h>
#include <plearn/io/pl_log.h>
#include <plearn/io/load_and_save.h>
#include <plearn/io/openFile.h>
#include <plearn/io/openString.h>
#include "TypeFactory.h"

#include <algorithm>

namespace PLearn {
using namespace std;

PLEARN_IMPLEMENT_OBJECT(
    Object,
    "Base class for all high-level PLearn objects.",
    "Object exposes simple mechanisms for:\n"
    "\n"
    "- automatic memory management (through reference counting and smart pointers)\n"
    "- serialization (read, write, save, load)\n"
    "- runtime type information (classname)\n"
    "- displaying (info, print)\n"
    "- deep copying (deepCopy)\n"
    "- a generic way of setting options (setOption) when not knowing the\n"
    "  exact type of the Object and a generic build() method (the combination\n"
    "  of the two allows to change the object structure and rebuild it at\n"
    "  runtime)\n"
    );


//#####  Basic PLearn::Object Protocol  #######################################

Object::Object()
{ }

Object::~Object()
{}

// by default, do nothing...
void Object::makeDeepCopyFromShallowCopy(CopiesMap& copies)
{}

void Object::build_()
{}

void Object::build()
{}

string Object::info() const { return classname(); }


//#####  Option-Manipulation Functions  #######################################

void Object::setOption(const string& optionname, const string& value)
{
    PStream in = openString(value, PStream::plearn_ascii);
    readOptionVal(in, optionname);
}

string Object::getOption(const string &optionname) const
{ 
    string s;
    PStream out = openString(s, PStream::plearn_ascii);
    writeOptionVal(out, optionname);
    return removeblanks(s);
}

void Object::changeOptions(const map<string,string>& name_value)
{
    map<string,string>::const_iterator it = name_value.begin();
    map<string,string>::const_iterator itend = name_value.end();
    while(it!=itend)
    {
        setOption(it->first, it->second);
        ++it;
    }
}

void Object::changeOption(const string& optionname, const string& value)
{
    map<string,string> name_value;
    name_value[optionname] = value;
    changeOptions(name_value);
}


//#####  Object::readOptionVal  ###############################################

void Object::readOptionVal(PStream &in, const string &optionname)
{
    try 
    {
        OptionList &options = getOptionList();
        for (OptionList::iterator it = options.begin(); it != options.end(); ++it) 
        {
            if ((*it)->optionname() == optionname) 
            {
                (*it)->read(this, in);
                return;
            }
        }

        // Found no options matching 'optionname'. First look for brackets. If there
        // are brackets, they must be located before any dot.
        size_t lb_pos = optionname.find('[');
        size_t rb_pos = optionname.find(']');
        size_t dot_pos = optionname.find('.');
        if (rb_pos != string::npos) {
            if (lb_pos == string::npos)
                PLERROR("Object::readOptionVal() - Unmatched brackets");
            string optname = optionname.substr(0, lb_pos);

            // Found no dot, or a right bracket before a dot
            if (dot_pos == string::npos || rb_pos < dot_pos) {
                string index = optionname.substr(lb_pos + 1, rb_pos - lb_pos - 1);
                for (OptionList::iterator it = options.begin(); it != options.end(); ++it)
                    if ((*it)->optionname() == optname) 
                    {
                        // There are two cases here: either there is a dot located
                        // immediately after the right bracket, or there is no dot.
                        // If there is a dot, the option HAS to be an Object
                        if (dot_pos != string::npos && dot_pos == rb_pos+1) {
                            int i = toint(index);
                            (*it)->getIndexedObject(this, i)->readOptionVal(
                                in, optionname.substr(dot_pos + 1));
                        }
                        else if (dot_pos == string::npos)
                            (*it)->readIntoIndex(this, in, index);
                        else
                            PLERROR("Object::readOptionVal() - unknown option format '%s'",
                                    optionname.c_str());
                        return;
                    }
            }
        }
        else if (lb_pos != string::npos)
            PLERROR("Object::readOptionVal() - Unmatched brackets");

        // No brackets, look for a dot
        if (dot_pos != string::npos) 
        {
            // Found a dot, assume it's a field with an Object * field
            string optname = optionname.substr(0, dot_pos);
            string optoptname = optionname.substr(dot_pos + 1);
            for (OptionList::iterator it = options.begin(); it != options.end(); ++it)
                if ((*it)->optionname() == optname) 
                {
                    (*it)->getAsObject(this)->readOptionVal(in, optoptname);
                    return;
                }
        }
    }
    catch(const PLearnError& e)
    { 
        PLERROR("Problem while attempting to read value of option %s of a %s:\n %s", 
                optionname.c_str(), classname().c_str(), e.message().c_str()); 
    }

    // There are bigger problems in the world but still it isn't always funny
    PLERROR("There is no option named %s in a %s", optionname.c_str(),classname().c_str());
}


//#####  Object::writeOptionVal  ##############################################

void Object::writeOptionVal(PStream &out, const string &optionname) const
{
    OptionList &options = getOptionList();
    for (OptionList::iterator it = options.begin(); it != options.end(); ++it)
        if ((*it)->optionname() == optionname) {
            (*it)->write(this, out);
            return;
        }

    // Found no options matching 'optionname'. First look for brackets. If there
    // are brackets, they must be located before any dot.
    size_t lb_pos = optionname.find('[');
    size_t rb_pos = optionname.find(']');
    size_t dot_pos = optionname.find('.');
    if (rb_pos != string::npos) {
        if (lb_pos == string::npos)
            PLERROR("Object::writeOptionVal() - Unmatched brackets");
        string optname = optionname.substr(0, lb_pos);

        // Found no dot, or a right bracket before a dot
        if (dot_pos == string::npos || rb_pos < dot_pos) {
            string index = optionname.substr(lb_pos + 1, rb_pos - lb_pos - 1);
            for (OptionList::iterator it = options.begin(); it != options.end(); ++it)
                if ((*it)->optionname() == optname) {
                    // There are two cases here: either there is a dot located
                    // immediately after the right bracket, or there is no dot.  If
                    // there is a dot, the option HAS to be an Object
                    if (dot_pos != string::npos && dot_pos == rb_pos+1) {
                        int i = toint(index);
                        (*it)->getIndexedObject(this, i)->writeOptionVal(
                            out, optionname.substr(dot_pos + 1));
                    }
                    else if (dot_pos == string::npos)
                        (*it)->writeAtIndex(this, out, index);
                    else
                        PLERROR("Object::writeOptionVal() - unknown option format '%s'",
                                optionname.c_str());
                    return;
                }
        }
    }
    else if (lb_pos != string::npos)
        PLERROR("Object::writeOptionVal() - Unmatched brackets");

    // No brackets, look for a dot
    if (dot_pos != string::npos) {
        // Found a dot, assume it's a field with an Object * field
        string optname = optionname.substr(0, dot_pos);
        string optoptname = optionname.substr(dot_pos + 1);
        for (OptionList::iterator it = options.begin(); it != options.end(); ++it)
            if ((*it)->optionname() == optname) {
                (*it)->getAsObject(this)->writeOptionVal(out, optoptname);
                return;
            }
    }
    // There are bigger problems in the world but still it isn't always funny
    PLERROR("Object::writeOptionVal() - Unknown option \"%s\"", optionname.c_str());    
}


//#####  Object::getOptionsToSave  ############################################

string Object::getOptionsToSave() const
{
    string res = "";
    OptionList& options = getOptionList();
  
    for( OptionList::iterator it = options.begin(); it!=options.end(); ++it )
    {
        OptionBase::flag_t flags = (*it)->flags();
        if(!(flags & OptionBase::nosave))
            res += (*it)->optionname() + " ";
    }
    return res;
}


//#####  Object::newread  #####################################################

void Object::newread(PStream &in)
{
    PP<Object> dummy_obj = 0; // Used to read skipped options.
    string cl;

    // Allow the use of the pointer syntax for non-pointer instances.
    in.skipBlanksAndComments();
    if ( in.peek() == '*' )  
    {
        const char* errmsg = "In Object::newread(PStream&) Wrong format. "
            "Expecting \"*%d->\" but got \"*%d%c%c\".";
    
        in.get(); // Eat '*'
        unsigned int id;
        in >> id;
        in.skipBlanksAndComments();

        char dash = in.get(); // Eat '-'
        if ( dash != '-' )
        {
            if ( dash == ';' )
                PLERROR("In Object::newread(PStream&): Non pointer objects can be prefixed with dummy "
                        "references ('*%d ->') but these references MUST NOT BE USED afterwards. Just "
                        "read '*%d;'", id, id );
            PLERROR( errmsg, id, id, dash, in.get() );
        }
    
        char cc = in.get();
        if(cc != '>') // Eat '>'
            PLERROR( errmsg, id, id, dash, cc);
        in.skipBlanksAndCommentsAndSeparators();    
    }
    
    in.getline(cl, '(');
    cl = removeblanks(cl);
    if (cl != classname())
        PLERROR("Object::newread() - Was expecting \"%s\", but read \"%s\"",
                classname().c_str(), cl.c_str());

    in.skipBlanksAndComments();
    int c = in.get();
    if (c != ')') 
    {
        in.putback(c);
        for (;;) 
        {
            // Read all specified options
            string optionname;
            in.getline(optionname, '=');
            optionname = removeblanks(optionname);
            in.skipBlanksAndComments();

            OptionList &options = getOptionList();
            OptionList::iterator it =
                find_if(options.begin(), options.end(),
                        bind2nd(mem_fun(&OptionBase::isOptionNamed), optionname));
            // if (it != options.end() && ((*it)->flags() & in.option_flags_in) == 0)
            if (it!=options.end() && (*it)->shouldBeSkipped() ) {
                // Create a dummy object that will read this option.
                if (!dummy_obj) {
                    dummy_obj = new Object();
                    PStream dummy_in = openString(this->classname() + "()", PStream::plearn_ascii);
                    dummy_in >> dummy_obj;
                }
                dummy_obj->readOptionVal(in, optionname);
            }
            else
            {
                // cerr << "Reading option: " << optionname << endl;
                readOptionVal(in, optionname);
                // cerr << "returned from reading optiion " << optionname << endl;
            }
            in.skipBlanksAndCommentsAndSeparators();
            /*
              in.skipBlanksAndCommentsAndSeparators();
              in.skipBlanksAndCommentsAndSeparators();
              in.skipBlanksAndCommentsAndSeparators();
              in.skipBlanksAndCommentsAndSeparators();
              cerr << "PEEK1: " << in.peek() << endl;
              in.peek(); in.peek(); in.peek();
              cerr << "PEEK2: " << in.peek() << endl;
            */

            if (in.peek() == ')') 
            {
                in.get();
                break;
            }
        }
    }
    build(); // Build newly read Object
}


//#####  Object::newwrite  ####################################################

void Object::newwrite(PStream &out) const
{
    vector<string> optnames = split(getOptionsToSave());
    out.write(classname());
    out.write("(\n");
    for (size_t i = 0; i < optnames.size(); ++i) 
    {
        out.write(optnames[i]);
        out.write(" = ");
        writeOptionVal(out, optnames[i]);
        if (i < optnames.size() - 1)
            out.write(";\n");
    }
    out.write(" )\n");
}


void Object::write(ostream& out_) const
{
    PStream out(&out_);
    newwrite(out);
}

void Object::read(istream& in_)
{ 
    in_ >> ws; // skip blanks
    int c = in_.peek();
    if(c=='<') // --> it's an "old-style" <Classname> ... </Classname> kind of definition
        oldread(in_);
    else { // assume it's a "new-style" Classname(...) 
        PStream in(&in_);
        newread(in);
    }
}


//#####  Remote Method Invocation  ############################################

void Object::prepareToSendResults(PStream& out, int nres)
{ 
    DBG_LOG << "PREPARING TO SEND " << nres << " RESULTS." << endl;
    out.write("!R "); out << nres; 
}


void Object::call(const string& methodname, int nargs, PStream& io)
{
    if(methodname=="changeOptions")
    {
        if(nargs!=1) PLERROR("Object remote method changeOptions takes 1 argument");
        map<string,string> name_value;
        io >> name_value;
        changeOptions(name_value);
        prepareToSendResults(io, 0);
        io.flush();
    }
    else if(methodname=="getOption") // gets option in serialised form
    {
        if(nargs!=1) PLERROR("Object remote method getOption takes 1 argument");
        string optionname;
        io >> optionname;
        prepareToSendResults(io, 1);
        writeOptionVal(io, optionname);
        io.flush();      
    }
    else if(methodname=="getOptionAsString") // gets option as a serialised string containing the serialised option
    {
        if(nargs!=1) PLERROR("Object remote method getOptionasString takes 1 argument");
        string optionname;
        io >> optionname;
        string optionval = getOption(optionname);
        prepareToSendResults(io, 1);
        io << optionval;
        io.flush();      
    }
    else if(methodname=="getObject")
    {
        if(nargs!=0) PLERROR("Object remote method getObject takes 0 arguments");
        prepareToSendResults(io, 1);
        io << *this;
        io.flush();      
    }
    else if(methodname=="save")
    {
        if(nargs!=2)
            PLERROR("Object remote method save takes 2 arguments: filepath, io_formatting");
        string filepath, io_formatting;
        io >> filepath >> io_formatting;
        if(io_formatting=="plearn_ascii")
            PLearn::save(filepath, *this, PStream::plearn_ascii);
        else if(io_formatting=="plearn_binary")
            PLearn::save(filepath, *this, PStream::plearn_binary);
        else
            PLERROR("In Object remote method save: invalid io_formatting %s",
                    io_formatting.c_str());
        prepareToSendResults(io, 0);
        io.flush();
    }
    else if(methodname=="run")
    {
        if(nargs!=0) PLERROR("Object remote method run takes 0 arguments");
        run();
        prepareToSendResults(io, 0);
        io.flush();
    }
    else
        PLERROR("In Object::call no method named %s supported by this object's call method.",
                methodname.c_str());
}


//#####  Serialization and Miscellaneous  #####################################

void Object::run()
{ PLERROR("Not a runnable Object"); }

void Object::oldread(istream& in)
{ PLERROR("oldread method not implemented for this object"); }

void Object::save(const PPath& filename) const
{
    PLWARNING("This method is deprecated. It simply calls the generic PLearn save function "
              "(that can save any PLearn object): PLearn::save(filename, *this) "
              "So you should call PLearn::save directly (it's defined in plearn/io/load_and_save.h).");
    PLearn::save(filename, *this);
}

void Object::load(const PPath& filename)
{
    PLWARNING("This method is deprecated. It simply calls the generic PLearn load function "
              "(that can load any PLearn object): PLearn::load(filename, *this) "
              "So you should call PLearn::load directly (it's defined in plearn/io/load_and_save.h).");
    PLearn::load(filename, *this);
}

Object* loadObject(const PPath &filename)
{
#if STREAMBUFVER == 1
    PStream in = openFile(filename, PStream::plearn_ascii, "r");
#else
    ifstream in_(filename.c_str());
    if (!in_)
        PLERROR("loadObject() - Could not open file \"%s\" for reading", filename.c_str());
    PStream in(&in_);
#endif
    Object *o = readObject(in);
    o->build();
    return o;
}

Object* macroLoadObject(const PPath &filename, map<string, string>& vars)
{
    string script = readFileAndMacroProcess(filename, vars);
    PStream sin = openString(script,PStream::plearn_ascii);
    Object* o = readObject(sin);
    o->build();
    return o;
}
  
Object* macroLoadObject(const PPath &filename)
{
    map<string, string> vars;
    return macroLoadObject(filename,vars);
}

Object* readObject(PStream &in, unsigned int id)
{
    Object *o=0;
    in.skipBlanksAndCommentsAndSeparators();

    //pl_streambuf* buf = dynamic_cast<pl_streambuf*>(in.rdbuf());
#if STREAMBUFVER == 0
    pl_streammarker fence(in.pl_rdbuf());
#endif

    string head;

    int c = in.peek();
    if (c == '<')  // Old (deprecated) serialization mode 
        PLERROR("Old deprecated serialization mode starting with '<' no longer supported.");
    else if (c == '*') // Pointer to object
    {
        in >> o;
    } 
    else if(c == '`') // back-quote: reference to an object in another file
    {
        in.get(); // skip the opening back-quote
        string fname;
        in.getline(fname,'`');
        fname = removeblanks(fname);
        // TODO: Check if this is really what we want
        //       (ie: We could want to use the options
        //            of 'in' to load the object...)
        o = loadObject(fname);
    }
    else // It must be a Classname(...) kind of definition 
    {
        string cl;
        in.getline(cl, '(');
        cl = removeblanks(cl);
        // It's a Classname(opt1 = ...; ...; optn = ...); --> calls newread()
        o = TypeFactory::instance().newObject(cl);
        if (!o)
            PLERROR("readObject() - Type \"%s\" not declared in TypeFactory map (did you do a proper DECLARE_NAME_AND_DEEPCOPY?)", cl.c_str());
#if STREAMBUFVER == 0
        in.pl_rdbuf()->seekmark(fence);
#else
        in.unread(cl+'(');
#endif
        o->newread(in);
    }

#if 0
    // Code that could be used... but need to see if it's useful and not too ugly.
    // See if we actually want an option of this object, instead of the object itself.
    in.skipBlanksAndCommentsAndSeparators();
    while (in.peek() == '.') {
        in.get(); // Skip the dot.
        char ch;
        string option_name;
        while (((ch = in.peek()) >= 'A' && ch <= 'z') || ch == '_' || (ch >= '0' && ch <= '9')) {
            in.get();
            option_name += ch;
        }
        if (option_name == "")
            PLERROR("In readObject - Could not read correctly the option name following a dot");
        o = newObject(o->getOption(option_name));
    }
#endif
       
    if (id != UINT_MAX)
        in.copies_map_in[id] = o;
    return o;
}


PStream& operator>>(PStream& in, Object*& x)
{
    in.skipBlanksAndCommentsAndSeparators();
    if (in.peek() == '*')
    {
        in.get(); // Eat '*'
        unsigned int id;
        in >> id;
        in.skipBlanksAndCommentsAndSeparators();
        if (id==0)
            x = 0;
        else if (in.peek() == '-') 
        {
            in.get(); // Eat '-'
            char cc = in.get();
            if(cc != '>') // Eat '>'
                PLERROR("In PStream::operator>>(Object*&)  Wrong format. "
                        "Expecting \"*%d->\" but got \"*%d-%c\".", id, id, cc);
            in.skipBlanksAndCommentsAndSeparators();
            if(x)
                in >> *x;
            else // x is null
                x = readObject(in, id);
            // in.skipBlanksAndCommentsAndSeparators();
            in.copies_map_in[id]= x;
        } 
        else 
        {
            // Find it in map and return ptr;
            map<unsigned int, void *>::iterator it = in.copies_map_in.find(id);
            if (it == in.copies_map_in.end())
                PLERROR("In PStream::operator>>(Object*&) object (ptr) to be read has not been previously defined");
            x= static_cast<Object *>(it->second);
        }
    } 
    else
    {
        x = readObject(in);
        // in.skipBlanksAndCommentsAndSeparators();
    }

    return in;
}



/*
  PStream& operator>>(PStream &in, Object * &o)
  {
  if (in.peek() == '*') {
  in.get(); // Eat '*'
  unsigned int id;
  in >> raw >> id;
  if (in.peek() == '-') {
  in.get(); // Eat '-'
  in.get(); // Eat '>'
            // Read object
            // NOTE: Object is added immediately to the map before any build() is called on it.
            o = readObject(in, id);
            } else {
            // Find it in map and return ptr
            map<unsigned int, void *>::iterator it = in.copies_map_in.find(id);
            if (it == in.copies_map_in.end())
            PLERROR("read() - Object to be read has not been previously defined");
            o = static_cast<Object *>(it->second);
            }
            } else {
            o = readObject(in);
            }
            return in;
            }
*/

} // end of namespace PLearn


//! Useful function for debugging inside gdb:
extern "C"
void printobj(PLearn::Object* p)
{
    PLearn::PStream perr(&std::cerr);
    perr << *p;
    perr.endl();
}


/*
  Local Variables:
  mode:c++
  c-basic-offset:4
  c-file-style:"stroustrup"
  c-file-offsets:((innamespace . 0)(inline-open . 0))
  indent-tabs-mode:nil
  fill-column:79
  End:
*/
// vim: filetype=cpp:expandtab:shiftwidth=4:tabstop=8:softtabstop=4:encoding=utf-8:textwidth=79 :
