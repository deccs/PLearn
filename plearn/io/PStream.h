// -*- C++ -*-
 
// PStream.h
// Copyright (C) 1998 Pascal Vincent
// Copyright (C) 1999-2001 Pascal Vincent, Yoshua Bengio and University of Montreal
// Copyright (C) 2002 Frederic Morin, Xavier Saint-Mleux, Pascal Vincent
//
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

#ifndef PStream_INC
#define PStream_INC

#include <map>
#include <set>
#include <sstream>
#include <fstream>
#include <plearn/base/byte_order.h>
#include <plearn/base/pl_hash_fun.h>
#include <plearn/base/plerror.h>
#include "PStream_util.h"
#include "PStreamBuf.h"
#include "StdPStreamBuf.h"

namespace PLearn {

using namespace std;

/*!
 * PStream:
 *  This class defines a type of stream that should be used for all I/O within PLearn.
 *  It supports most operations available for standard c++ streams, plus:
 *   - a set of option flags that indicate which types of Object Options should be
 *     read/written (option_flags_{in|out}; has effect only for Object's);
 *   - a set of mode flags that define format used for I/O. e.g.:  "raw_ascii" for
 *     standard c++ stream behaviour, "plearn_ascii"  for human-readable
 *     serialization format, etc. (inmode and outmode);
 *   - a copies map to allow smart serialization of pointers;
 *   - a markable stream buffer which allows to 'seek back' to a previously set mark
 *     on any type of istream;
 *   - an 'attach' function to attach the stream to a POSIX file descriptor;
 */

class PStream 
#if STREAMBUFVER == 0
: public PP<StdPStreamBuf>
#elif  STREAMBUFVER == 1
: public PP<PStreamBuf>
#endif

{
public:
  //! typedef's for PStream manipulators
  typedef PStream& (*pl_pstream_manip)(PStream&);

private:

#if STREAMBUFVER == 0
  typedef PP<StdPStreamBuf> inherited;
  typedef StdPStreamBuf streambuftype;
#elif  STREAMBUFVER == 1
  typedef PP<PStreamBuf> inherited;
  typedef PStreamBuf streambuftype;
#endif

public:

// norman: check for win32
#if __GNUC__ < 3 && !defined(WIN32)
  //! typedef's for compatibility w/ old libraries
  typedef int streamsize;
  typedef ios ios_base;
#endif

  enum mode_t 
    {
      plearn_ascii,    //<! PLearn ascii serialization format (can be mixed with plearn_binary)
      plearn_binary,   //<! PLearn binary serialization format (can be mixed with plearn_ascii)
      raw_ascii,       //<! Raw C++ ascii output without additional separators (direct output to underlying ostream)
      raw_binary,      //<! Simply writes the bytes as they are in memory.
      pretty_ascii     //<! Ascii pretty print (in particular for Vec and Mat, formatted output without size info)
    };
  
  //! plearn_ascii and plearn_binary are used on output to determine in which format to write stuff.
  //! On input however, they are equivalent, as the right format is automatically detected.

  //! Compression mode (mostly used by binary serialization of sequences of floats or doubles, such as TMat<real>)
  //! (Used on output only; autodetect on read).
  enum compr_mode_t { 
    compr_none,            //<! No compression.
    compr_double_as_float, //<! In plearn_binary mode, store doubles as float
    compr_sparse,          //<! PLearn 
    compr_lossy_sparse     //<! Also stores double as float 
  };

public:  
  mode_t inmode;              //<! mode for input formatting
  // bitset<32> pl_stream_flags_in;  //<! format flags for input
  map<unsigned int, void *> copies_map_in; //<! copies map for input
  mode_t outmode;            //<! mode for output formatting
  // bitset<32> pl_stream_flags_out; //<! format flags for output
  map<void *, unsigned int> copies_map_out; //<! copies map for output

private:
  static char tmpbuf[100];
  
public:
  //! If true, then Mat and Vec will be serialized with their elements in place,
  //! If false, they will have an explicit pointer to a storage
  bool implicit_storage;
  //! Determines the way data is compressed, if any.
  compr_mode_t compression_mode;

public:  

  PStream();

  //! constructor from a PStreamBuf
  PStream(streambuftype* sb);

  //! ctor. from an istream (I)
  PStream(istream* pin_, bool own_pin_=false);

  //! ctor. from an ostream (O)
  PStream(ostream* pout_, bool own_pout_=false);

  //! ctor. from an iostream (IO)
  PStream(iostream* pios_, bool own_pios_=false);

  //! ctor. from an istream and an ostream (IO)
  PStream(istream* pin_, ostream* pout_, bool own_pin_=false, bool own_pout_=false);

  //! Default copy ctor. should be fine now.

  //! Destructor.
  virtual ~PStream();

  inline void setBufferCapacities(streamsize inbuf_capacity, streamsize outbuf_capacity, streamsize unget_capacity)
  { ptr->setBufferCapacities(inbuf_capacity, outbuf_capacity, unget_capacity); }

  inline void setInMode(mode_t m) { inmode = m; }
  inline void setOutMode(mode_t m) { outmode = m; }
  inline void setMode(mode_t m) { inmode = m; outmode = m; }

  PStream& operator>>(mode_t m) { inmode = m; return *this; }
  PStream& operator<<(mode_t m) { outmode = m; return *this; }

public:
  //op()'s: re-init with different underlying stream(s)

  PStream& operator=(const PStream& pios);
  PStream& operator=(streambuftype* streambuf)
  { inherited::operator=(streambuf); return *this; }


#if STREAMBUFVER == 0
  inline PStream& operator()(istream* pin)
  { ptr->setIn(pin); return *this; }

  inline PStream& operator()(ostream* pout)
  { ptr->setOut(pout);  return *this; }

  inline PStream& operator()(iostream* pios)
  { 
    ptr->setIn(pios); 
    ptr->setOut(pios);  
    return *this; 
  }

  inline PStream& operator()(istream* pin, ostream* pout)
  { 
    ptr->setIn(pin); 
    ptr->setOut(pout);  
    return *this; 
  }

  inline PStream& operator=(istream* pin) { return operator()(pin); }
  inline PStream& operator=(ostream* pout)  { return operator()(pout); }
  inline PStream& operator=(iostream* pios)  { return operator()(pios); }

  inline PStream& operator()(const PStream& pios)  { return operator=(pios); }
#endif



  void writeAsciiNum(char x);
  void writeAsciiNum(unsigned char x);
  void writeAsciiNum(signed char x);
  void writeAsciiNum(short x);
  void writeAsciiNum(unsigned short x);
  void writeAsciiNum(int x);
  void writeAsciiNum(unsigned int x);
  void writeAsciiNum(long x);
  void writeAsciiNum(unsigned long x);
  void writeAsciiNum(float x);
  void writeAsciiNum(double x);

  void readAsciiNum(char &x);
  void readAsciiNum(unsigned char &x);
  void readAsciiNum(signed char &x);
  void readAsciiNum(short &x);
  void readAsciiNum(unsigned short &x);
  void readAsciiNum(int &x);
  void readAsciiNum(unsigned int &x);
  void readAsciiNum(long &x);
  void readAsciiNum(unsigned long &x);
  void readAsciiNum(float &x);
  void readAsciiNum(double &x);

  //! Writes the corresponding 2 hex digits (ex: 0A )
  void writeAsciiHexNum(unsigned char x);

  inline bool eof() const 
  { 
#if STREAMBUFVER == 0 
    return (*this)->rawin()->eof(); 
#elif STREAMBUFVER == 1
    return ptr->eof();
#endif
  }

#if STREAMBUFVER == 0
  inline bool good() const
  { return ptr->rawin() && ptr->rawin()->good() || ptr->rawout() && ptr->rawout()->good(); }

  //! op bool: true if the stream is in a valid state (e.g. "while(stm) stm >> x;")
  // This implementation does not seem to work: commented out [Pascal]
  // inline operator bool() { return (!pin || *pin) && (!pout || *pout) && (pin || pout); }
  // This is a temporary fix [Pascal]
  inline operator bool() 
  { return good(); }

  //! DEPRECATED access to underlying istream
  inline istream& _do_not_use_this_method_rawin_() 
  { return *(ptr->rawin()); }   
#else
  bool good() const
  { return ptr->good(); }

  operator bool() const
    { return good(); }
#endif
  
  /******
   * The folowing methods are 'forwarded' from {i|o}stream.
   */
  inline int get() 
  {
#if STREAMBUFVER == 1 
    return ptr->get();
#else
    return ptr->rawin()->get(); 
#endif
  }

  inline PStream& get(char& c) 
  { 
#if STREAMBUFVER == 1 
    c = (char)ptr->get();
#else
    ptr->rawin()->get(c); 
#endif
    return *this; 
  }

  //! Delimitor is read from stream but not appended to string. 
  inline PStream& getline(string& line, char delimitor='\n')
  { 
#if STREAMBUFVER == 1 
    line.clear();
    int c = get();
    while (c != EOF && c != delimitor)
      {
        line += (char)c;
        c = get();
      }
#else
    std::getline(*ptr->rawin(), line, delimitor); 
#endif
    return *this; 
  }

  inline string getline()
  { string s; getline(s); return s; }

  inline int peek() 
  { 
#if STREAMBUFVER == 1 
    return ptr->peek();
#else
    // return ptr->rawin()->peek() does not seem to work, so we use this [Pascal]
    int c = ptr->rawin()->get(); 
    ptr->rawin()->unget(); 
    return c; 
#endif
  }
  
  inline PStream& putback(char c) 
  { 
#if STREAMBUFVER == 1 
    ptr->putback(c);
#else
    ptr->rawin()->putback(c); 
#endif
    return *this; 
  }

#if STREAMBUFVER == 1 
  // We do not define unget. Use putback(c) instead.
#else
  inline PStream& unget() { ptr->rawin()->unget(); return *this; }
#endif

  inline PStream& unread(const char* p, streamsize n)
  {
#if STREAMBUFVER == 1 
    ptr->unread(p,n);
#else
    PLERROR("unread not supported for STREAMBUFVER==0");
#endif
    return *this;
  }

  inline PStream& unread(const char* p)
  { return unread(p,strlen(p)); }

  inline PStream& unread(const string& s)
  { return unread(s.data(),s.length()); }


  inline PStream& read(char* s, streamsize n) 
  { 
#if STREAMBUFVER == 1 
    ptr->read(s,n);
    return *this;
#else
    // The following line does not Work!!!! [Pascal]
    //    ptr->rawin()->read(s,n);
    // So it's temporarily replaced by this (ugly and slow):
    while (n)
    {
      int c = get();
      if (c == EOF) break;
      *s++ = (char) c;
      n--;
    }
    return *this; 
#endif
  }

  inline PStream& read(string& s, streamsize n) 
  {
    char* buf = new char[n];
#if STREAMBUFVER == 1
    streamsize nread = ptr->read(buf, n);
    s.assign(buf, nread);
#else
    read(buf, n);
    s.assign(buf, n);
#endif
    delete[] buf;
    return *this;
  }

  //! Reads characters into buf until n characters have been read, or end-of-file has been reached, 
  //! or the next character in the stream is the stop_char.
  //! Returns the total number of characters put into buf.
  //! The stopping character met is not extracted from the stream.
  streamsize readUntil(char* buf, streamsize n, char stop_char);

  //! Reads characters into buf until n characters have been read, or end-of-file has been reached, 
  //! or the next character in the stream is one of the stop_chars (null terminated string)
  //! Returns the total number of characters put into buf.
  //! The stopping character met is not extracted from the stream.
  streamsize readUntil(char* buf, streamsize n, const char* stop_chars);

  inline PStream& write(const char* s, streamsize n) 
  { 
#if STREAMBUFVER == 1 
    ptr->write(s,n);
#else
    ptr->rawout()->write(s,n); 
#endif
    return *this; 
  }

  inline PStream& put(char c) 
  { 
#if STREAMBUFVER == 1 
    ptr->put(c);
#else
    ptr->rawout()->put(c);
#endif
    return *this;
  }

  inline PStream& put(unsigned char c)
    {
      write(reinterpret_cast<char *>(&c), sizeof(c));
      return *this;
    }
  inline PStream& put(int x) { return put((char)x); }

  inline PStream& flush() 
  { 
#if STREAMBUFVER == 1 
    ptr->flush();
#else
    ptr->rawout()->flush();
#endif
    return *this; 
  }

  inline PStream& endl()
    {
      put('\n');
      flush();
      return *this;
    }

  // These are convenient method for writing raw strings (whatever the outmode):
  inline PStream& write(const char* s) 
  { 
    write(s, strlen(s));
    return *this; 
  }

  inline PStream& write(const string& s) 
  { 
    write(s.data(),s.length());
    return *this; 
  }

#if STREAMBUFVER == 1 
  // not the way to do it woth new PStreamBuf
#else
  //! attach this stream to a POSIX file descriptor.
  inline void attach(int fd)
    {
      ptr->attach(fd);
    }
#endif

  // Useful skip functions

  //! reads everything until '\n' (also consumes the '\n')
  void skipRestOfLine();

  //! skips any blanks (space, tab, newline)
  void skipBlanks();

  //! skips any blanks (space, tab, newline) and comments starting with #
  void skipBlanksAndComments();

  //! skips any blanks, # comments, and separators (',' and ';')
  void skipBlanksAndCommentsAndSeparators();

  //! skips all occurences of any of the given characters
  void skipAll(const char* chars_to_skip);

  //! Reads characters from stream, until we meet one of the stopping symbols at the current "level".
  //! i.e. any opening parenthesis, bracket, brace or quote will open a next level and we'll 
  //! be back to the current level only *after* we meet the corresponding closing parenthesis, 
  //! bracket, brace or quote.
  //! All characters read, except the stoppingsymbol, will be *appended* to characters_read 
  //! The stoppingsymbol is read and returned, but not appended to characters_read.
  //! Comments starting with # until the end of line may be skipped (as if they were not part of the stream)
  int smartReadUntilNext(const string& stoppingsymbols, string& characters_read,
                         bool ignore_brackets=false, bool skip_comments=true);

  //! Count the number of occurrences of a character in the stream.
  int count(char c);

  // operator>>'s for base types
  PStream& operator>>(bool &x);
  PStream& operator>>(float &x);
  PStream& operator>>(double &x);
  PStream& operator>>(string &x);
  PStream& operator>>(char* x); // read string in already allocated char[]
  PStream& operator>>(char &x); 
  PStream& operator>>(signed char &x);
  PStream& operator>>(unsigned char &x);
  PStream& operator>>(int &x);
  PStream& operator>>(unsigned int &x);  
  PStream& operator>>(long &x);  
  PStream& operator>>(unsigned long &x);
  PStream& operator>>(short &x);
  PStream& operator>>(unsigned short &x);
  PStream& operator>>(pl_pstream_manip func) { return (*func)(*this); }

  // operator<<'s for base types
  PStream& operator<<(float x);
  PStream& operator<<(double x);

  //! Warning: string output will be formatted according to outmode
  //! (if you want to output a raw string use the write method instead)
  PStream& operator<<(const char *x);

  //! Warning: string output will be formatted according to outmode
  //! (if you want to output a raw string use the write method instead)
  //! (unless you're in raw_ascii or raw_binary mode!)
  PStream& operator<<(const string &x);

  PStream& operator<<(char x); 
  PStream& operator<<(signed char x);
  PStream& operator<<(unsigned char x);

  // PStream& operator<<(bool x);  // If you uncomment this, be prepared to get mysterious mesages of problems with const bool resolutions
  // As it currently stands, bool will be converted to 0 or 1 int and operator<<(int) will get called.
  PStream& operator<<(int x);
  PStream& operator<<(unsigned int x);
  PStream& operator<<(long x);
  PStream& operator<<(unsigned long x);
  PStream& operator<<(short x);
  PStream& operator<<(unsigned short x);
  PStream& operator<<(pl_pstream_manip func) { return (*func)(*this); }
 
#if STREAMBUFVER == 0
   //! returns the markable input buffer
  //! DEPRECATED: TO BE REMOVED SOON, DO NOT USE!
  inline pl_streambuf* pl_rdbuf() { return ptr->pl_rdbuf(); }
#endif

};

/*! PStream objects to replace the standard cout, cin, ... */
PStream& get_pin();
PStream& get_pout();
PStream& get_pio();
PStream& get_perr();

extern PStream pin;
extern PStream pout;
extern PStream pio;
extern PStream perr;

  // Simulation of <<flush <<endl and >>ws ...

  extern PStream& flush(PStream& out);
  extern PStream& endl(PStream& out);
  extern PStream& ws(PStream& out);

  // But inject the standard ones as well to keep them usable!!!
  using std::flush;
  using std::endl;
  using std::ws;

  
  /*****
   * op>> & op<< for generic pointers
   */

  template <class T> 
  inline PStream& operator>>(PStream& in, T*& x)
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
              PLERROR("In PStream::operator>>(T*&)  Wrong format.  Expecting \"*%d->\" but got \"*%d-%c\".", id, id, cc);
            in.skipBlanksAndCommentsAndSeparators();
            if(!x)
              x= new T();
            in >> *x;
            in.skipBlanksAndCommentsAndSeparators();
            in.copies_map_in[id]= x;
          } 
        else 
          {
            // Find it in map and return ptr;
            map<unsigned int, void *>::iterator it = in.copies_map_in.find(id);
            if (it == in.copies_map_in.end())
              PLERROR("In PStream::operator>>(T*&) object (ptr) to be read has not been previously defined");
            x= static_cast<T *>(it->second);
          }
      } 
    else
      {
        in >> *x;
        in.skipBlanksAndCommentsAndSeparators();
      }

    return in;
  }


  template <class T> 
  inline PStream& operator<<(PStream& out, const T*& x)
  {
    if(x)
      {
        map<void *, unsigned int>::iterator it = out.copies_map_out.find(const_cast<T*&>(x));
        if (it == out.copies_map_out.end()) 
          {
            int id = (int)out.copies_map_out.size()+1;
            out.put('*');
            out << id;
            out.write("->");
            out.copies_map_out[const_cast<T*&>(x)] = id;
            out << *x;
          }
        else 
          {
            out.put('*');
            out << it->second;
            out.put(' ');
          }
      }
    else
      out.write("*0 ");
    return out;
  }

  template <class T> 
  inline PStream& operator>>(PStream& in, PP<T> &o)
  {
    T *ptr;
    if (o.isNull())
      ptr = 0;
    else
      ptr = o;
    in >> ptr;
    o = ptr;
    return in;
  }

  template <class T> 
  inline PStream& operator<<(PStream& out, const PP<T> &o)
  {
    T *ptr = static_cast<T *>(o);
    out << const_cast<const T * &>(ptr);
    return out;
  }

  template <class T> 
  inline PStream& operator<<(PStream& out, T*& ptr)
  {
    out << const_cast<const T * &>(ptr);
    return out;
  }

inline PStream& operator<<(PStream& out, bool x) 
{ 
  switch(out.outmode)
  {
    case PStream::raw_binary:
    case PStream::raw_ascii:
    case PStream::pretty_ascii:
      if(x) 
        out.put('1'); 
      else 
        out.put('0');
      break;
    case PStream::plearn_ascii:
      if(x) 
        out.put('1'); 
      else 
        out.put('0');
      out.put(' ');
      break;
    case PStream::plearn_binary:
      out.put((char)0x12);
      if(x) 
        out.put('1'); 
      else 
        out.put('0');
      break;
    default:
      PLERROR("In PStream::operator<<  unknown outmode!!!!!!!!!");
      break;
  }
  return out;
}

  


// Serialization of pairs in the form:   
// first : second

template<class A,class B>
inline PStream& operator<<(PStream& out, const pair<A,B>& x) 
{ 
  out << x.first;
  out.write(": ");
  out << x.second;
  out.put(' ');
  return out;
}

template <typename S, typename T> 
inline PStream& operator>>(PStream& in, pair<S, T> &x) 
{ 
  in.skipBlanksAndCommentsAndSeparators();
  in >> x.first;
  in.skipBlanksAndComments();
  if(in.get()!=':')
    PLERROR("In operator>>(PStream& in, pair<S, T> &x) expected ':' to separate the 2 halves of the pair");
  in.skipBlanksAndComments();
  in >> x.second;
  return in;
}


// Serialization of map types

template<class MapT>
void writeMap(PStream& out, const MapT& m)
{  
  typename MapT::const_iterator it = m.begin();
  typename MapT::const_iterator itend = m.end();

  out.put('{');
  if(!m.empty())
  {
    // write the first item
    out << it->first;
    out.write(": ");
    out << it->second;
    ++it;
    while(it!=itend)
    {
      out.write(", ");
      out << it->first;
      out.write(": ");
      out << it->second;
      ++it;
    }
  }
  out.put('}');    
}

template<class MapT>
void readMap(PStream& in, MapT& m)
{
  m.clear();
  in.skipBlanksAndCommentsAndSeparators();
  int c = in.get();
  if(c!='{')
    PLERROR("In readMap(Pstream& in, MapT& m) expected '{' but read %c",c);
  in.skipBlanksAndCommentsAndSeparators();
  c = in.peek(); // do we have a '}' ?
  while(c!='}')
    {
      pair<typename MapT::key_type, typename MapT::mapped_type> val;
      in >> val.first;
      in.skipBlanksAndCommentsAndSeparators();
      c = in.get();
      if(c!=':')
        PLERROR("In readMap(Pstream& in, MapT& m) separator between key and value must be ':', but I read a '%c'",c);
      in.skipBlanksAndCommentsAndSeparators();
      in >> val.second;
      m.insert(val);
      in.skipBlanksAndCommentsAndSeparators();
      c = in.peek(); // do we have a '}' ?
    }
  in.get(); // eat the '}'  
}

template<class Key, class Value, class Compare, class Alloc>
inline PStream& operator<<(PStream& out, const map<Key, Value, Compare, Alloc>& m)
{ writeMap(out, m); return out; }

template<class Key, class Value, class Compare, class Alloc>
inline PStream& operator>>(PStream& in, map<Key, Value, Compare, Alloc>& m)
{ readMap(in, m); return in; }

template<class Key, class Value, class Compare, class Alloc>
inline PStream& operator<<(PStream& out, const multimap<Key, Value, Compare, Alloc>& m)
{ writeMap(out, m); return out; }

template<class Key, class Value, class Compare, class Alloc>
inline PStream& operator>>(PStream& in, multimap<Key, Value, Compare, Alloc>& m)
{ readMap(in, m); return in; }


template<class Key, class Value, class Compare, class Alloc>
inline PStream& operator<<(PStream& out, const hash_map<Key, Value, Compare, Alloc>& m)
{ writeMap(out, m); return out; }

template<class Key, class Value, class Compare, class Alloc>
inline PStream& operator>>(PStream& in, hash_map<Key, Value, Compare, Alloc>& m)
{ readMap(in, m); return in; }

template<class Key, class Value, class Compare, class Alloc>
inline PStream& operator<<(PStream& out, const hash_multimap<Key, Value, Compare, Alloc>& m)
{ writeMap(out, m); return out; }

template<class Key, class Value, class Compare, class Alloc>
inline PStream& operator>>(PStream& in, hash_multimap<Key, Value, Compare, Alloc>& m)
{ readMap(in, m); return in; }


/** Serialization of sequences **/
/* These methods are there only to simplify the writing of operator<< and operator>> and
   should not be called by user code directly */

template<class Iterator>
void binwrite_(PStream& out, Iterator& it, unsigned int n)
{
  PStream::mode_t outmode = out.outmode; // store previous outmode
  if(outmode!=PStream::raw_binary && outmode!=PStream::plearn_binary)
    out.outmode = PStream::plearn_binary;
  while(n--)
    {
      out << *it;
      ++it;
    }
  out.outmode = outmode; // restore previous outmode 
}

inline void binwrite_(PStream& out, const bool* x, unsigned int n)
{
  while(n--)
    {
      if(*x++)
        out.put('1');
      else
        out.put('0');
    }
}

inline void binwrite_(PStream& out, const char* x, unsigned int n) 
{ out.write((char*)x, n*sizeof(char)); }
inline void binwrite_(PStream& out, char* x, unsigned int n) 
{ out.write((char*)x, n*sizeof(char)); }

inline void binwrite_(PStream& out, const signed char* x, unsigned int n) 
{ out.write((char*)x, n*sizeof(signed char)); }
inline void binwrite_(PStream& out, signed char* x, unsigned int n) 
{ out.write((char*)x, n*sizeof(signed char)); }

inline void binwrite_(PStream& out, const unsigned char* x, unsigned int n) 
{ out.write((char*)x, n*sizeof(unsigned char)); }
inline void binwrite_(PStream& out, unsigned char* x, unsigned int n) 
{ out.write((char*)x, n*sizeof(unsigned char)); }

inline void binwrite_(PStream& out, const short* x, unsigned int n) 
{ out.write((char*)x, n*sizeof(short)); }
inline void binwrite_(PStream& out, short* x, unsigned int n) 
{ out.write((char*)x, n*sizeof(short)); }

inline void binwrite_(PStream& out, const unsigned short* x, unsigned int n) 
{ out.write((char*)x, n*sizeof(unsigned short)); }
inline void binwrite_(PStream& out, unsigned short* x, unsigned int n) 
{ out.write((char*)x, n*sizeof(unsigned short)); }

inline void binwrite_(PStream& out, const int* x, unsigned int n) 
{ out.write((char*)x, n*sizeof(int)); }
inline void binwrite_(PStream& out, int* x, unsigned int n) 
{ out.write((char*)x, n*sizeof(int)); }

inline void binwrite_(PStream& out, const unsigned int* x, unsigned int n) 
{ out.write((char*)x, n*sizeof(unsigned int)); }
inline void binwrite_(PStream& out, unsigned int* x, unsigned int n) 
{ out.write((char*)x, n*sizeof(unsigned int)); }

inline void binwrite_(PStream& out, const long* x, unsigned int n) 
{ out.write((char*)x, n*sizeof(long)); }
inline void binwrite_(PStream& out, long* x, unsigned int n) 
{ out.write((char*)x, n*sizeof(long)); }

inline void binwrite_(PStream& out, const unsigned long* x, unsigned int n) 
{ out.write((char*)x, n*sizeof(unsigned long)); }
inline void binwrite_(PStream& out, unsigned long* x, unsigned int n) 
{ out.write((char*)x, n*sizeof(unsigned long)); }

inline void binwrite_(PStream& out, const float* x, unsigned int n) 
{ out.write((char*)x, n*sizeof(float)); }
inline void binwrite_(PStream& out, float* x, unsigned int n) 
{ out.write((char*)x, n*sizeof(float)); }

inline void binwrite_(PStream& out, const double* x, unsigned int n) 
{ out.write((char*)x, n*sizeof(double)); }
inline void binwrite_(PStream& out, double* x, unsigned int n) 
{ out.write((char*)x, n*sizeof(double)); }

// The typecode indicates the type and format of the elements in the stream

template<class Iterator>
void binread_(PStream& in, Iterator it, unsigned int n, unsigned char typecode)
{
  if(typecode!=0xFF)
    PLERROR("In binread_ : bug! A specialised binread_ should have been called for a typecode other than the 'generic' 0xFF");

  while(n--)
    {
      in >> *it;
      ++it;
    }
}

void binread_(PStream& in, bool* x, unsigned int n, unsigned char typecode);

inline void binread_(PStream& in, char* x,
              unsigned int n, unsigned char typecode)  
{                                                      
  // big endian and little endian have the same typecodes
  // so we need to check only one for consistency

  if(typecode!=TypeTraits<char>::little_endian_typecode()
     && typecode!=TypeTraits<unsigned char>::little_endian_typecode()) 
    PLERROR("In binread_ incompatible typecode");      

  in.read((char*)x, n);
}

inline void binread_(PStream& in, signed char* x, unsigned int n, unsigned char typecode)
{ binread_(in, (char *)x, n, typecode); }

inline void binread_(PStream& in, unsigned char* x, unsigned int n, unsigned char typecode)
{ binread_(in, (char *)x, n, typecode); }

void binread_(PStream& in, short* x, unsigned int n, unsigned char typecode);
void binread_(PStream& in, unsigned short* x, unsigned int n, unsigned char typecode);
void binread_(PStream& in, int* x, unsigned int n, unsigned char typecode);
void binread_(PStream& in, unsigned int* x, unsigned int n, unsigned char typecode);
void binread_(PStream& in, long* x, unsigned int n, unsigned char typecode);
void binread_(PStream& in, unsigned long* x, unsigned int n, unsigned char typecode);
void binread_(PStream& in, float* x, unsigned int n, unsigned char typecode);
void binread_(PStream& in, double* x, unsigned int n, unsigned char typecode);


template<class SequenceType>
void writeSequence(PStream& out, const SequenceType& seq)
{
  // norman: added explicit cast
  unsigned int n = (unsigned int)seq.size();
  typename SequenceType::const_iterator it = seq.begin();
  
  switch(out.outmode)
    {
    case PStream::raw_ascii:      
      while(n--)
        {
          out << *it;
          out.put(' ');
          ++it;
        }
      break;
      
    case PStream::pretty_ascii:
      out.write("[ ");
      while(n--)
        {
          out << *it;
          if(n>0)
            out.write(", ");
          ++it;
        }
      out.write(" ] ");
      break;

    case PStream::raw_binary: 
      binwrite_(out, it, n);
      break;

    case PStream::plearn_ascii:
      out << n;
      out.write("[ ");
      while(n--)
        {
          out << *it;
          ++it;
        }
      out.write("] ");
      break;

    case PStream::plearn_binary:
      {
        unsigned char typecode;
        if(byte_order()==LITTLE_ENDIAN_ORDER)
          {
            out.put((char)0x12); // 1D little-endian 
            typecode = TypeTraits<typename SequenceType::value_type>::little_endian_typecode();
          }
        else
          {
            out.put((char)0x13); // 1D big-endian
            typecode = TypeTraits<typename SequenceType::value_type>::big_endian_typecode();
          }

        // write typecode
        out.put(typecode);
        
        // write length in raw_binary 
        out.write((char*)&n, sizeof(n));
        
        // write the data
        binwrite_(out, it, n);
      }
      break;
      
    default:
      PLERROR("In PStream::writeSequence(Iterator& it, int n)  unknown outmode!!!!!!!!!");
      break;
    }
}


//! Reads in a sequence type from a PStream.
/*! For this to work with the current implementation, the SequenceType must have:
  - typedefs defining (SequenceType::...) value_type, size_type, iterator 
  - a begin() method that returns a proper iterator,
  - a size_type size() method returning the size of the current container
  - a resize(size_type n) method that allows to change the size of the container
  (which should also work with resize(0) )
  - a push_back(const value_type& x) method that appends the element x at the end
*/
template<class SequenceType>
void readSequence(PStream& in, SequenceType& seq)
{
  switch(in.inmode)
    {
    case PStream::raw_ascii:
      {
	    // norman: added explicit cast
        int n = (int)seq.size();
        typename SequenceType::iterator it = seq.begin();
        while(n--)
          {
            in >> *it; 
            in.skipBlanks();
            ++it;
          }
      }
      break;
    case PStream::raw_binary:
      {
        int n = (int)seq.size();
        typename SequenceType::iterator it = seq.begin();
        while(n--)
          {
            in >> *it; 
            ++it;
          }
      }
      break;

    case PStream::plearn_ascii:
    case PStream::plearn_binary:
      {
        in.skipBlanksAndComments();
        int c = in.peek();
        if(c=='[') // read until ']'
          {
            in.get(); // skip '['
            in.skipBlanksAndCommentsAndSeparators();
            seq.resize(0);
            while(in.peek()!=']' && in.peek()!=EOF && !in.eof())
              {
                typename SequenceType::value_type x;
                in >> x;
                seq.push_back(x);
                in.skipBlanksAndCommentsAndSeparators();
              }
            if (in.peek()==EOF || in.eof())
              PLERROR("Reading stream, unmatched left bracket [, missing ]");
            in.get(); // skip ']'
          }
        else if(isdigit(c))
          {
            unsigned int n;
            in >> n;
            in.skipBlanksAndComments();
            c = in.get();
            if(c!='[')
              PLERROR("Error in readSequence(SequenceType& seq), expected '[', read '%c'",c);
            in.skipBlanksAndCommentsAndSeparators();
            seq.resize((typename SequenceType::size_type) n);
            if (n>0)
            {
              typename SequenceType::iterator it = seq.begin();
              while(n--)
              {
                in >> *it;
                in.skipBlanksAndCommentsAndSeparators();
                ++it;
              }
            }
            in.skipBlanksAndCommentsAndSeparators();
            c = in.get();
            if(c!=']')
              PLERROR("Error in readSequence(SequenceType& seq), expected ']', read '%c'",c);

          }
        else if(c==0x12 || c==0x13) // it's a generic binary 1D sequence
          {
            in.get(); // eat c
            unsigned char typecode = in.get(); 
            unsigned int l;
            in.read((char*)&l,sizeof(l));

            bool inverted_byte_order = (    (c==0x12 && byte_order()==BIG_ENDIAN_ORDER) 
                                         || (c==0x13 && byte_order()==LITTLE_ENDIAN_ORDER) );

            if(inverted_byte_order)
              endianswap(&l);
            seq.resize((typename SequenceType::size_type) l);
            binread_(in, seq.begin(), l, typecode);
          }
        else
          PLERROR("In readSequence(SequenceType& seq) '%c' not a proper first character in the header of a sequence!",c);
      }
      break;

    default:
      PLERROR("In PStream::operator>>  unknown inmode!!!!!!!!!");
      break;
    }
    
}

// Default behavior for write() and read() is
// to call corresponding operator<<() or operator>>()
// on PStream.

template<class T> 
inline void write(ostream& out_, const T& o)
{
  PStream out(&out_);
  out << o;
}

template<class T> 
inline void read(istream& in_, T& o)
{
  PStream in(&in_);
  in >> o;
}

template<class T> 
inline void read(const string& stringval, T& x)
{
  istringstream in_(stringval);
  PStream in(&in_);
  in >> x;
}


// STL containers:

template <class T> inline PStream &
operator>>(PStream &in, vector<T> &v)
{ readSequence(in, v); return in; }

template <class T> inline PStream &
operator<<(PStream &out, const vector<T> &v)
{ writeSequence(out, v); return out; }

// Serialization of map types

template<class SetT>
void writeSet(PStream& out, const SetT& s)
{  
  typename SetT::const_iterator it = s.begin();
  typename SetT::const_iterator itend = s.end();

  out.put('[');
  while(it!=itend)
    {
      out << *it;
      ++it;
      if (it != itend)
          out.write(", ");
    }
  out.put(']');
}

template<class SetT>
void readSet(PStream& in, SetT& s)
{
  s.clear();
  in.skipBlanksAndCommentsAndSeparators();
  int c = in.get();
  if(c!='[')
    PLERROR("In readSet(Pstream& in, SetT& s) expected '[' but read %c",c);
  in.skipBlanksAndCommentsAndSeparators();
  c = in.peek(); // do we have a ']' ?
  while(c!=']')
    {
      typename SetT::value_type val;
      in >> val;
      in.skipBlanksAndCommentsAndSeparators();
      s.insert(val);
      c = in.peek(); // do we have a ']' ?
    }
  in.get(); // eat the ']'
}

template <class T> inline PStream &
operator>>(PStream &in, set<T> &v)
{ readSet(in, v); return in; }

template <class T> inline PStream &
operator<<(PStream &out, const set<T> &v)
{ writeSet(out, v); return out; }


/// @deprected Use openFile instead.
class PIFStream: public PStream
{
public:
  PIFStream(const string& fname, ios_base::openmode m = ios_base::in)
    :PStream(new ifstream(fname.c_str()),true) 
  {
    PLDEPRECATED("PIFStream is deprecated. Use the openFile function instead.");
  }
};

/// @deprecated Use openFile instead.
class POFStream: public PStream
{
public:
  POFStream(const string& fname, ios_base::openmode m = ios_base::out | ios_base::trunc)
    :PStream(new ofstream(fname.c_str()),true) 
  {
    PLDEPRECATED("POFStream is deprecated. Use the openFile function instead.");
  }
};


/// @deprecated Use openString instead.
class PIStringStream: public PStream
{
public:
  PIStringStream(const string& s)
    :PStream(new istringstream(s), true /* own it */) 
  {
    PLDEPRECATED("PIStringStream is deprecated. Use the openString function instead.");
  }
};


} // namespace PLearn

#endif //ndef PStream_INC
