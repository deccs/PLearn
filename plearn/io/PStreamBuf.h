// -*- C++ -*-

// PStreamBuf.h
// 
// Copyright (C) 2003 Pascal Vincent
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

/* *******************************************************      
   * $Id: PStreamBuf.h,v 1.13 2005/01/28 17:43:03 plearner Exp $ 
   ******************************************************* */

/*! \file PStreamBuf.h */
#ifndef PStreamBuf_INC
#define PStreamBuf_INC

#define STREAMBUFVER 1

#include <plearn/base/PP.h>

namespace PLearn {
using namespace std;

class PStreamBuf: public PPointable
{    
public:

  // typedef Object inherited;
  typedef PPointable inherited;
  typedef size_t streamsize; 
  typedef off_t streampos; 

  // ****************
  // * Constructors *
  // ****************

  //! Default constructor
  PStreamBuf(bool is_readable_, bool is_writable_,
             streamsize inbuf_capacity=1, streamsize outbuf_capacity=0,
             streamsize unget_capacity=default_ungetsize);

  void setBufferCapacities(streamsize inbuf_capacity,
                           streamsize outbuf_capacity,
                           streamsize unget_capacity);

  virtual ~PStreamBuf();

protected:

  /// Default size for unget buffer for PStreamBuf and its subclasses.
  static const streamsize default_ungetsize = 100;

  bool is_readable;
  bool is_writable;

private: 

  // Input buffer mechanism
  // ungetsize+inbuf_chunksize characters are allocated in total for the buffer.
  // Calls to read_ are always made as read_(inbuf+ungetsize, inbuf_chunksize);
  // The first ungetsize characters of the buffer are reserved for ungets
  streamsize ungetsize;
  streamsize inbuf_chunksize;
  char* inbuf;  //!< beginning of input buffer
  char* inbuf_p; //!< position of next character to be read
  char* inbuf_end; //!< one after last available character

  // Output buffer
  streamsize outbuf_chunksize;
  char* outbuf; //!< beginning of output buffer
  char* outbuf_p; //!< position of next character to be written
  char* outbuf_end; //!< one after last reserved character in outbuf

protected:
  //! reads up to n characters into p
  //! You should override this call in subclasses. 
  //! Default version issues a PLERROR
  /*!  On success, the number of bytes read is returned.  Zero indicates
    end of file. If we are not at end of file, at least one character
    should be returned (the call must block until at least one char is
    available).  It is not an error if the number returned is smaller than
    the number of bytes requested; this may happen for example because
    fewer bytes are actually available right now (maybe because we were
    close to end-of-file, or because we are reading from a pipe, or from a
    terminal).  If an error occurs, an exception should be thrown.
  */
  virtual streamsize read_(char* p, streamsize n);

  //! writes exactly n characters from p (unbuffered, must flush)
  //! Default version issues a PLERROR
  virtual void write_(const char* p, streamsize n);

private:

  // refills the inbuf
  streamsize refill_in_buf();

public:

  bool isReadable() const
  { return is_readable; }

  bool isWritable() const
  { return is_writable; }

  int get()
  {
    if(inbuf_p<inbuf_end || refill_in_buf())
      return *inbuf_p++;
    else
      return -1;
  }
  
  //! character c will be returned by the next get()
  void putback(char c);

  int peek()
  {
    if(inbuf_p<inbuf_end || refill_in_buf())
      return *inbuf_p;
    else
      return -1;
  }
  
  streamsize read(char* p, streamsize n);

  //! puts the given characters back in the input buffer
  //! so that they're the next thing read.
  void unread(const char* p, streamsize n);

  void flush();

  void put(char c)
  {
#ifdef BOUNDCHECK
  if(!isWritable())
    PLERROR("Called PStreamBuf::put on a buffer not marked as writable");
#endif
  if(outbuf_chunksize>0) // buffered
    {
      if(outbuf_p==outbuf_end)
        flush();
      *outbuf_p++ = c;
    }
  else // unbuffered
    write_(&c,1);
  }

  void write(const char* p, streamsize n);

  /// Checks if the streambuf is valid and can be written to or read from.
  virtual bool good() const;

  /// Checks if we reached the end of the file.
  bool eof() const
  {
    return const_cast<PStreamBuf*>(this)->peek() == EOF;
  }
  
};

} // end of namespace PLearn

#endif
