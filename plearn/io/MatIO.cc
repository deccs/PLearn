// -*- C++ -*-

// PLearn (A C++ Machine Learning Library)
// Copyright (C) 1998 Pascal Vincent
// Copyright (C) 1999-2002 Pascal Vincent, Yoshua Bengio and University of Montreal
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
   * $Id: MatIO.cc,v 1.4 2004/02/29 16:44:05 nova77 Exp $
   * This file is part of the PLearn library.
   ******************************************************* */

#include "MatIO.h"
#include "stringutils.h"
#include "fileutils.h"

namespace PLearn {
using namespace std;


/* 
A word on the old PLearn native binary file format for matrices and vectors (.pmat .pvec):

The file starts with a 64 bytes ASCII ascii header, directly followed by
the data in binary format (a simple memory dump, 4 bytes per value for
floats and 8 bytes for doubles).  The ascii header gives the data type, the
dimensions, and the type of binary representation, as shown in the examples
below. The remaining of the 64 bytes is filled with white spaces (' ') and
a final newline ('\n') The 64th byte must always be a newline, this will be
checked by the loading code.

The data in these files can be memory mapped if desired.  Notice that
memory-mapping requires that the architecture has the same memory
representation (big endian or little endian) that is used in the file, as
specified in the header (see below) Otherwise you'll get an error message
telling you of the problem.  However traditional loading does not require
this; load will 'translate' the representation if necessary.

Examples of header lines:

A 100 element vector of floats (4bytes each) in little endian binary representation:
VECTOR 100 FLOAT LITTLE_ENDIAN

A 100 element vector of doubles (8 bytes each) in big endian binary representation:
VECTOR 100 double BIG_ENDIAN

A 20x10 matrix of doubles in little endian binary representation
MATRIX 20 10 DOUBLE LITTLE_ENDIAN

A 20x10 matrix of floats in big endian binary representation
MATRIX 20 10 FLOAT BIG_ENDIAN

As a convention, we will use the following file extensions: .pmat and .pvec
(p stands for PLearn) (or possibly .lpmat .bpmat and .lpvec .bpvec if you
wish to keep both a little endian and a big endian version of the matrix in
the same place, to allow memory-mapping on different platforms) 
*/

void loadMat(const string& file_name, TMat<float>& mat)
{
  string ext = extract_extension(file_name);
  // See if we know the extension...
  if(ext==".amat")
    loadAscii(file_name, mat);
  else if (ext==".pmat" || ext==".lpmat" || ext==".bpmat")
    loadPMat(file_name,mat);
  else // try to guess the format from the header
  {
    ifstream in(file_name.c_str());
    if(!in)
      PLERROR("In loadMat: could not open file %s",file_name.c_str());      
    char c = in.get();
    in.close();
    if(c=='M') // it's most likely a .pmat format
      loadPMat(file_name,mat);
    else 
      loadAscii(file_name,mat);
  }
}

void loadMat(const string& file_name, TMat<double>& mat)
{
  string ext = extract_extension(file_name);
  // See if we know the extension...
  if(ext==".amat")
    loadAscii(file_name, mat);
  else if (ext==".pmat" || ext==".lpmat" || ext==".bpmat")
    loadPMat(file_name,mat);
  else // try to guess the format from the header
  {
    ifstream in(file_name.c_str());
    if(!in)
      PLERROR("In loadMat: could not open file %s",file_name.c_str());      
    char c = in.get();
    in.close();
    if(c=='M') // it's most likely a .pmat format
      loadPMat(file_name,mat);
    else 
      loadAscii(file_name,mat);
  }
}


void loadVec(const string& file_name, TVec<float>& vec)
{
  const char* filename = file_name.c_str();
  char* suffix = strrchr(filename,'.');
  if (!suffix || strcmp(suffix,".avec")==0)
    loadAscii(file_name, vec);
  else if (strcmp(suffix,".pvec")==0 || strcmp(suffix,".lpvec")==0 || strcmp(suffix,".bpvec")==0)
    loadPVec(file_name,vec);
  else
    PLERROR("In loadVec: unknown file extension");
}

void loadVec(const string& file_name, TVec<double>& vec)
{
  const char* filename = file_name.c_str();
  char* suffix = strrchr(filename,'.');
  if (!suffix || strcmp(suffix,".avec")==0)
    loadAscii(file_name, vec);
  else if (strcmp(suffix,".pvec")==0 || strcmp(suffix,".lpvec")==0 || strcmp(suffix,".bpvec")==0)
    loadPVec(file_name,vec);
  else
    PLERROR("In loadVec: unknown file extension");
}

// Old native PLearn format (.pvec and .pmat)
// DATAFILE_HEADER_LENGTH is 64 and is defined in general.h

void savePVec(const string& filename, const TVec<float>& vec)
{
  FILE* f = fopen(filename.c_str(),"w");
  if (!f)
    PLERROR("In savePVec, could not open file %s for writing",filename.c_str());

  char header[DATAFILE_HEADERLENGTH]; 

#ifdef LITTLEENDIAN
  sprintf(header,"VECTOR %d FLOAT LITTLE_ENDIAN", vec.length());
#endif
#ifdef BIGENDIAN
  sprintf(header,"VECTOR %d FLOAT BIG_ENDIAN", vec.length());
#endif

  // Pad the header with whites and terminate it with '\n'
  for(int pos=(int)strlen(header); pos<DATAFILE_HEADERLENGTH; pos++)
    header[pos] = ' ';
  header[DATAFILE_HEADERLENGTH-1] = '\n';

  // write the header to the file
  fwrite(header,DATAFILE_HEADERLENGTH,1,f);

  // write the data to the file
  if(0 < vec.length())
  {
    const float* p = vec.data();
    fwrite(p,sizeof(float),vec.length(),f);
  }

  fclose(f);
}

void savePVec(const string& filename, const TVec<double>& vec)
{
  FILE* f = fopen(filename.c_str(),"w");
  if (!f)
    PLERROR("In savePVec, could not open file %s for writing",filename.c_str());

  char header[DATAFILE_HEADERLENGTH]; 

#ifdef LITTLEENDIAN
  sprintf(header,"VECTOR %d DOUBLE LITTLE_ENDIAN", vec.length());
#endif
#ifdef BIGENDIAN
  sprintf(header,"VECTOR %d DOUBLE BIG_ENDIAN", vec.length());
#endif

  // Pad the header with whites and terminate it with '\n'
  for(int pos=(int)strlen(header); pos<DATAFILE_HEADERLENGTH; pos++)
    header[pos] = ' ';
  header[DATAFILE_HEADERLENGTH-1] = '\n';

  // write the header to the file
  fwrite(header,DATAFILE_HEADERLENGTH,1,f);

  // write the data to the file
  if(0 < vec.length())
  {
    const double* p = vec.data();
    fwrite(p,sizeof(double),vec.length(),f);
  }

  fclose(f);
}

void loadPVec(const string& filename, TVec<float>& vec)
{
  char header[DATAFILE_HEADERLENGTH];
  char matorvec[20];
  char datatype[20];
  char endiantype[20];
  int the_length;

  FILE* f = fopen(filename.c_str(),"r");
  if (!f)
    PLERROR("In loadPVec, could not open file %s for reading",filename.c_str());
  fread(header,DATAFILE_HEADERLENGTH,1,f);
  if(header[DATAFILE_HEADERLENGTH-1]!='\n')
    PLERROR("In loadPVec(%s), wrong header for PLearn binary vector format. Please use checkheader (in PLearn/Scripts) to check the file.",filename.c_str());
  sscanf(header,"%s%d%s%s",matorvec,&the_length,datatype,endiantype);
  if (strcmp(matorvec,"VECTOR")!=0)
    PLERROR("In loadPVec(%s), wrong header for PLearn binary vector format. Please use checkheader (in PLearn/Scripts) to check the file.",filename.c_str());

  vec.resize(the_length);

  bool is_file_bigendian = false;
  if (strcmp(endiantype,"LITTLE_ENDIAN")==0)
    is_file_bigendian = false;
  else if (strcmp(endiantype,"BIG_ENDIAN")==0)
    is_file_bigendian = true;
  else
    PLERROR("In loadPVec, wrong header for PLearn binary vector format. Please use checkheader (in PLearn/Scripts) to check the file.");

  if (strcmp(datatype,"FLOAT")==0)
  {
    float* p = vec.data();
    fread_float(f,p,vec.length(),is_file_bigendian);
  }

  else if (strcmp(datatype,"DOUBLE")==0)
  {
    double* buffer = new double[vec.length()];
    float* p = vec.data();
    fread_double(f,buffer,vec.length(),is_file_bigendian);
    for(int j=0; j<vec.length(); j++)
      p[j] = float(buffer[j]);
    delete[] buffer;
  }

  else
    PLERROR("In loadPVec, wrong header for PLearn binary vector format. Please use checkheader (in PLearn/Scripts) to check the file.");

  fclose(f);
}

void loadPVec(const string& filename, TVec<double>& vec)
{
  char header[DATAFILE_HEADERLENGTH];
  char matorvec[20];
  char datatype[20];
  char endiantype[20];
  int the_length;

  FILE* f = fopen(filename.c_str(),"r");
  if (!f)
    PLERROR("In loadPVec, could not open file %s for reading",filename.c_str());
  fread(header,DATAFILE_HEADERLENGTH,1,f);
  if(header[DATAFILE_HEADERLENGTH-1]!='\n')
    PLERROR("In loadPVec(%s), wrong header for PLearn binary vector format. Please use checkheader (in PLearn/Scripts) to check the file.",filename.c_str());
  sscanf(header,"%s%d%s%s",matorvec,&the_length,datatype,endiantype);
  if (strcmp(matorvec,"VECTOR")!=0)
    PLERROR("In loadPVec(%s), wrong header for PLearn binary vector format. Please use checkheader (in PLearn/Scripts) to check the file.",filename.c_str());

  vec.resize(the_length);

  bool is_file_bigendian = false;
  if (strcmp(endiantype,"LITTLE_ENDIAN")==0)
    is_file_bigendian = false;
  else if (strcmp(endiantype,"BIG_ENDIAN")==0)
    is_file_bigendian = true;
  else
    PLERROR("In loadPVec, wrong header for PLearn binary vector format. Please use checkheader (in PLearn/Scripts) to check the file.");

  if (0 < the_length)
  {
    if (strcmp(datatype,"FLOAT")==0)
    {
      float* buffer = new float[vec.length()];
      double* p = vec.data();
      fread_float(f,buffer,vec.length(),is_file_bigendian);
      for(int j=0; j<vec.length(); j++)
        p[j] = double(buffer[j]);
      delete[] buffer;
    }

    else if (strcmp(datatype,"DOUBLE")==0)
    {
      double* p = vec.data();
      fread_double(f,p,vec.length(),is_file_bigendian);
    }
      
    else
      PLERROR("In loadPVec, wrong header for PLearn binary vector format. Please use checkheader (in PLearn/Scripts) to check the file.");
  }
  fclose(f);
}


void savePMat(const string& filename, const TMat<float>& mat)
{
  FILE* f = fopen(filename.c_str(),"w");
  if (!f)
    PLERROR("In savePMat, could not open file %s for writing",filename.c_str());

  char header[DATAFILE_HEADERLENGTH]; 

#ifdef LITTLEENDIAN
  sprintf(header,"MATRIX %d %d FLOAT LITTLE_ENDIAN", mat.length(), mat.width());
#endif
#ifdef BIGENDIAN
  sprintf(header,"MATRIX %d %d FLOAT BIG_ENDIAN", mat.length(), mat.width());
#endif

  // Pad the header with whites and terminate it with '\n'
  for(int pos=(int)strlen(header); pos<DATAFILE_HEADERLENGTH; pos++)
    header[pos] = ' ';
  header[DATAFILE_HEADERLENGTH-1] = '\n';

  // write the header to the file
  fwrite(header,DATAFILE_HEADERLENGTH,1,f);

  // write the data to the file
  for (int i=0; i<mat.length(); i++) 
  {
    const float* p = mat[i];
    fwrite(p,sizeof(float),mat.width(),f);
  }
  fclose(f);
}

void savePMat(const string& filename, const TMat<double>& mat)
{
  FILE* f = fopen(filename.c_str(),"w");
  if (!f)
    PLERROR("In savePMat, could not open file %s for writing",filename.c_str());

  char header[DATAFILE_HEADERLENGTH]; 

#ifdef LITTLEENDIAN
  sprintf(header,"MATRIX %d %d DOUBLE LITTLE_ENDIAN", mat.length(), mat.width());
#endif
#ifdef BIGENDIAN
  sprintf(header,"MATRIX %d %d DOUBLE BIG_ENDIAN", mat.length(), mat.width());
#endif

  // Pad the header with whites and terminate it with '\n'
  for(int pos=(int)strlen(header); pos<DATAFILE_HEADERLENGTH; pos++)
    header[pos] = ' ';
  header[DATAFILE_HEADERLENGTH-1] = '\n';

  // write the header to the file
  fwrite(header,DATAFILE_HEADERLENGTH,1,f);

  // write the data to the file
  for (int i=0; i<mat.length(); i++) 
  {
    const double* p = mat[i];
    fwrite(p,sizeof(double),mat.width(),f);
  }
  fclose(f);
}

void loadPMat(const string& filename, TMat<float>& mat)
{
  char header[DATAFILE_HEADERLENGTH];
  char matorvec[20];
  char datatype[20];
  char endiantype[20];
  int the_length;
  int the_width;

  FILE* f = fopen(filename.c_str(),"r");
  if (!f)
    PLERROR("In loadPMat, could not open file %s for reading",filename.c_str());
  fread(header,DATAFILE_HEADERLENGTH,1,f);
  if(header[DATAFILE_HEADERLENGTH-1]!='\n')
    PLERROR("In loadPMat(%s), wrong header for PLearn binary matrix format. Please use checkheader (in PLearn/Scripts) to check the file.",filename.c_str());
  sscanf(header,"%s%d%d%s%s",matorvec,&the_length,&the_width,datatype,endiantype);
  if (strcmp(matorvec,"MATRIX")!=0)
    PLERROR("In loadPMat(%s), wrong header for PLearn binary matrix format. Please use checkheader (in PLearn/Scripts) to check the file.",filename.c_str());

  mat.resize(the_length, the_width);

  bool is_file_bigendian = true;
  if (strcmp(endiantype,"LITTLE_ENDIAN")==0)
    is_file_bigendian = false;
  else if (strcmp(endiantype,"BIG_ENDIAN")==0)
    is_file_bigendian = true;
  else
    PLERROR("In loadPMat, wrong header for PLearn binary matrix format. Please use checkheader (in PLearn/Scripts) to check the file.");

  if (strcmp(datatype,"FLOAT")==0)
  {
    for (int i=0; i<mat.length(); i++)
    {
      float* p = mat[i];
      fread_float(f,p,mat.width(),is_file_bigendian);
    }
  }
  else if (strcmp(datatype,"DOUBLE")==0)
  {
    double* buffer = new double[mat.width()];
    for (int i=0; i<mat.length(); i++)
    {
      float* p = mat[i];
      fread_double(f,buffer,mat.width(),is_file_bigendian);
      for(int j=0; j<mat.width(); j++)
        p[j] = float(buffer[j]);
    }
    delete[] buffer;
  }

  else
    PLERROR("In loadPMat, wrong header for PLearn binary matrix format. Please use checkheader (in PLearn/Scripts) to check the file.");

  fclose(f);
}

void loadPMat(const string& filename, TMat<double>& mat)
{
  char header[DATAFILE_HEADERLENGTH];
  char matorvec[20];
  char datatype[20];
  char endiantype[20];
  int the_length;
  int the_width;

  FILE* f = fopen(filename.c_str(),"r");
  if (!f)
    PLERROR("In loadPMat, could not open file %s for reading",filename.c_str());
  fread(header,DATAFILE_HEADERLENGTH,1,f);
  if(header[DATAFILE_HEADERLENGTH-1]!='\n')
    PLERROR("In loadPMat(%s), wrong header for PLearn binary matrix format. Please use checkheader (in PLearn/Scripts) to check the file.",filename.c_str());
  sscanf(header,"%s%d%d%s%s",matorvec,&the_length,&the_width,datatype,endiantype);
  if (strcmp(matorvec,"MATRIX")!=0)
    PLERROR("In loadPMat(%s), wrong header for PLearn binary matrix format. Please use checkheader (in PLearn/Scripts) to check the file.",filename.c_str());

  mat.resize(the_length, the_width);

  bool is_file_bigendian = true;
  if (strcmp(endiantype,"LITTLE_ENDIAN")==0)
    is_file_bigendian = false;
  else if (strcmp(endiantype,"BIG_ENDIAN")==0)
    is_file_bigendian = true;
  else
    PLERROR("In loadPMat, wrong header for PLearn binary matrix format. Please use checkheader (in PLearn/Scripts) to check the file.");

  if (strcmp(datatype,"FLOAT")==0)
  {
    float* buffer = new float[mat.width()];
    for (int i=0; i<mat.length(); i++)
    {
      double* p = mat[i];
      fread_float(f,buffer,mat.width(),is_file_bigendian);
      for(int j=0; j<mat.width(); j++)
        p[j] = double(buffer[j]);
    }
    delete[] buffer;
  }

  else if (strcmp(datatype,"DOUBLE")==0)
  {
    for (int i=0; i<mat.length(); i++)
    {
      double* p = mat[i];
      fread_double(f,p,mat.width(),is_file_bigendian);
    }
  }

  else
    PLERROR("In loadPMat, wrong header for PLearn binary matrix format. Please use checkheader (in PLearn/Scripts) to check the file.");

  fclose(f);
}

/*
void newLoadAscii(const string& filename, TMat<double>& mat)
{
  ifstream in(filename.c_str());
  if(!in)
    PLERROR("Could not open file %s for reading", filename.c_str());

  int length = -1;
  int width = -1;
  bool could_be_old_amat=true; // true while there is still a chance that this be an "old" amat format (length and width in first row with no starting ##)
  int c = in.get();
  while(isspace(c))
    c = in.get();

  if(c=='#') // starts with a comment
  {
    could_be_old_amat = false;

    // If it's followed by another # and only 2 numbers before the end of line, it's a new .amat format
    if(in.get()=='#')
    {
      in >> length;
      in >> width;
    }
    c = in.get();
    while(c==' ' || c=='\t')
      c = in.get();
    if(c!='\n' && c!='\r') // it wasn't a new .amat format after all... 
      length = -1;
  }

  if(length==-1)  // still looking for size info...
  {      
    in.unget();
    string line;
    getNextNonBlankLine(in,line);
    int nfields1 = split(line).size();
    getNextNonBlankLine(in,line);
    int nfields2 = split(line).size();
    if(could_be_old_amat && nfields1==2) // could be an old .amat with first 2 numbers being length width
    {
      in.seekg(0);
      real a, b;
      in >> a >> b;
      if(real(int(a))==a && real(int(b))==b && a>0 && b>0 && int(b)==nfields2) // it's clearly an old .amat
      {
        length = int(a);
        width = int(b);
      }
    }

    if(length==-1) // still don't know size info...
    {
      if(nfields1==nfields2) // looks like a plain ascii file
      {
        length = countNonBlankLinesOfFile(filename);
        width = nfields1;
        in.seekg(0); 
      }
    }
  }

  if(length==-1)
    PLERROR("In loadAscii: couldn't determine file format automatically");

  mat.resize(length,width);
  for(int i=0; i<length; i++)
  {
    real* mat_i = mat[i];
    skipBlanks(in);
    for(int j=0; j<width; j++)
      in >> mat_i[j];
  }

}
*/

// Gnuplot format
void saveGnuplot(const string& filename, const Vec& vec)
{
  FILE* f=fopen(filename.c_str(),"w");
  if (!f) 
    PLERROR("In Vec::saveGnuplot, couldn't open %s for writing",filename.c_str());

  real* p = vec.data();
  for (int i=0; i<vec.length(); i++, p++)
    fprintf(f,"%e\n", *p);
  fclose(f);
}

// Gnuplot format
void saveGnuplot(const string& filename, const Mat& mat)
{
  ofstream out(filename.c_str());
  if (!out) 
    PLERROR("In saveGnuplot, couldn't open %s for writing.",filename.c_str());
  out.flags(ios::left);
  for(int i=0; i<mat.length(); i++)
  {
    const real* m_i = mat[i];
    for(int j=0; j<mat.width(); j++)
      out << setw(11) << m_i[j] << ' ';
    out << "\n";
  }
  out.flush();
}

void loadGnuplot(const string& filename, Mat& mat)
{
  ifstream in(filename.c_str());
  if (!in) 
    PLERROR("In loadGnuplot, couldn't open %s for reading.",filename.c_str());

  char buf[10000];
  // First pass to count the number of rows and columns
  int nrows = 0;
  int ncols = 0;
  in.getline(buf,sizeof(buf)-1);
  while(in)
  {
    int pos=0;
    while(buf[pos]==' ' || buf[pos]=='\t')
      pos++;
    if(buf[pos]!='#' && buf[pos]!='\n' && buf[pos]!='\r')
    {
      nrows++;
      if(ncols==0)
      {
        istrstream inputline(buf);
        real value;
        while(inputline)
        {
          inputline >> value;
          ncols++;
        }
        ncols--; // correct count
      }
    }
    in.getline(buf,sizeof(buf)-1);
  }
  in.close();
  mat.resize(nrows,ncols);
  in.open(filename.c_str());
  for(int i=0; i<nrows; i++)
  {
    char firstchar = '#';
    while(firstchar == '#' || firstchar == '\n' || firstchar=='\r')
    {
      in.getline(buf,sizeof(buf)-1);
      int pos=0;
      while(buf[pos]==' ' || buf[pos]=='\t')
        pos++;
      firstchar = buf[pos];
    }
    istrstream inputline(buf);      
    for(int j=0; j<ncols; j++)
      inputline >> mat(i,j);
  }
  in.close();
}

// Ascii without size
void saveAsciiWithoutSize(const string& filename, const Vec& vec)
{
  FILE *f;
  f=fopen(filename.c_str(),"w");
  if (!f)
    PLERROR("In Vec::saveAscii: could not open file %s for writing",filename.c_str());
  int i;
  char buffer[100];
  real *p= vec.data();
  for (i=0;i<vec.length();i++,p++)
  {
    pretty_print_number(buffer,*p);
    fprintf(f,"%s ",buffer);
  }
  fclose(f);
}

// Ascii without size (load assumes size is already correctly set)
void loadAsciiWithoutSize(const string& filename, const Vec& vec)
{
  FILE *f;
  f=fopen(filename.c_str(),"r");
  if (!f)
    PLERROR("In Vec::loadAsciiWithoutSize could not open file %s for reading",filename.c_str());

  if (vec.length() < 1)
    PLERROR("In Vec::loadAsciiWithoutSize, the size of the vector is not defined yet");

  real* p = vec.data();
  for (int i=0;i<vec.length();i++,p++)
  {
#ifdef USEDOUBLE
    fscanf(f,"%lf",p);
#else
    fscanf(f,"%f",p);
#endif
  }
}

void saveAsciiWithoutSize(const string& filename, const Mat& mat)
{
  FILE *f;
  f=fopen(filename.c_str(),"w");
  if (!f)
    PLERROR("In saveAscii, could not open file %s for writing",filename.c_str());
  char buffer[100];
  for(int i=0; i<mat.length(); i++) 
  {
    const real* row_i = mat[i];
    for(int j=0; j<mat.width(); j++)
    {
      pretty_print_number(buffer,row_i[j]);
      fprintf(f,"%s ",buffer);
    }
    fprintf(f,"\n");
  }
  fclose(f);
}

void loadAsciiWithoutSize(const string& filename, const Mat& mat)
{

  FILE *f = fopen(filename.c_str(),"r");
  if (!f)
    PLERROR("In loadAsciiWithoutSize, could not open file %s for reading.",filename.c_str());

  if (mat.length() < 1 || mat.width() < 1)
    PLERROR("In loadAsciiWithoutSize, the size of the matrix is not defined yet");

  for(int i=0; i<mat.length(); i++)
  {
    real* row_i = mat[i];
    for(int j=0; j<mat.width(); j++)
#ifdef USEDOUBLE
      fscanf(f,"%lf",&row_i[j]);
#else
      fscanf(f,"%f",&row_i[j]);
#endif
  }
}


// Native SN format (Fmat)
void saveSNMat(const string& filename, const Mat& mat)
{
  FILE *f=fopen(filename.c_str(),"w");
  int i=0x1e3d4c51L;
  int j=0;
  fwrite_int(f,&i,1);
  i=2; /*  number of dimensions = 2 for a matrix  */
  fwrite_int(f,&i,1);
  int length = mat.length();
  int width = mat.width();
  fwrite_int(f,&length,1);
  fwrite_int(f,&width,1);
  while (i++ < 3) fwrite_int(f,&j,1);
  for (i=0;i<length;i++)
    fwrite_float(f,mat[i],width);
  fclose(f);
}

Mat loadSNMat(const string& filename)
{
  FILE *f;
  int d, nd; int i;
  int imn;
  int length, width;

  f=fopen(filename.c_str(),"r");
  if (!f)
    PLERROR("In loadFmat, could not open file %s for reading",filename.c_str());

  fread_int(f,&imn,1);
  if (imn!= (0x1e3d4c51L))
    PLERROR("in loadFmat, File does not have the right format");

  /*  read ndim  */
  fread_int(f,&nd,1);
  if (nd<0 || nd>5)
    PLERROR("In loadFmat, Corrupted file: Bad number of dimensions");
  if (nd != 2)
    PLERROR("In loadFmat, ndim is not 2 (not a matrix!)\n");

  /*  read dim  */
  d=0;
  fread_int(f,&length,1);
  d++;
  fread_int(f,&width,1);
  d++;
  while (d++ < 3) 
    fread(&i, sizeof(int), 1, f);
  Mat mat(length,width);
  for(i=0; i<length; i++)
    fread_float(f, mat[i], width);
  fclose(f);
  return mat;
}

void saveSNVec(const string& filename, const Vec& vec)
{
  FILE* f=fopen(filename.c_str(),"w");
  if(!f)
    PLERROR("In Vec::loadSNVec could not open file for writing");
  int i=0x1e3d4c51L;
  int j=0;
  fwrite_int(f,&i,1);
  i=1; /*  number of dimensions = 1 for a vector  */
  fwrite_int(f,&i,1);
  int length = vec.length();
  fwrite_int(f,&length,1);
  while (i++ < 3) 
    fwrite_int(f,&j,1);
  fwrite_float(f,vec.data(),length);
  fclose(f);
}

Vec loadSNVec(const string& filename)
{
  FILE *f;
  int d, nd; int i;
  int imn;
  int size;

  f=fopen(filename.c_str(),"r");
  if (!f)
    PLERROR("In Vec::loadSNVec could not open file %s for reading",filename.c_str());

  fread_int(f,&imn,1);
  if (imn!= (0x1e3d4c51L))
    PLERROR("In Vec::loadSNVec, File does not have the right format");

  /*  read ndim  */
  fread_int(f,&nd,1);
  if (nd<0 || nd>5)
    PLERROR("In Vec::loadSNVec, Corrupted file: Bad number of dimensions");
  if (nd != 1)
    PLERROR("In Vec::loadSNVec, ndim is not 1 (not a vector!)\n");

  /*  read dim  */
  d=0;
  fread_int(f,&i,1);
  size=i;d++;
  while (d++ < 3) 
    fread(&i, sizeof(int), 1, f);

  Vec vec(size);
  fread_float(f,vec.data(),size);

  fclose(f);
  return vec;
}


// Native AD format

Mat loadADMat(const string& filename)
{
  FILE *f = fopen(filename.c_str(),"r");
  if (!f)
    PLERROR("In loadADMat, could not open file %s for reading",filename.c_str());
  int the_length, the_width;
  int magic = 0x2345;
  int SNidx2fltmagic = 0x0D02;
  int m;
  fread_int(f,&m,1);
  if (m != magic && m != SNidx2fltmagic)
    PLERROR("In load, magic number is incorrect: %d != %d",m,magic);
  fread_int(f,&the_length,1);
  fread_int(f,&the_width,1);
  Mat mat(the_length,the_width);
  fread_float(f,mat.data(),the_length*the_width);
  fclose(f);
  return mat;
}

Vec loadADVec(const string& filename)
{
  FILE* f = fopen(filename.c_str(),"r");
  if (!f)
    PLERROR("In Vec::loadADMat could not open file %s for reading",filename.c_str());
  int thesize;
  int magic = 0x3456;
  int m;
  fread_int(f,&m,1);
  if (m != magic)
    PLERROR("In new_Vec_from_File_FILE, magic number is incorret: %d != %d",m,magic);
  fread_int(f,&thesize,1);
  Vec vec(thesize);
  fread_float(f,vec.data(),thesize);
  fclose(f);
  return vec;
}

// Used for calling qsort
static int compare_string_pointers(const void *ts1, const void *ts2) 
{
  return strcmp(*((char **)ts1),*((char **)ts2));
}


// UCI machine-learning-database format
Mat loadUCIMLDB(const string& filename, char ****to_symbols, int **to_n_symbols)
{
  FILE *f = fopen(filename.c_str(),"r");
  int n_rows= -1, n_cols=1, i,j;
  char ***symbols;
  int *n_symbols;
#define convert_UCIMLDB_BUF_LEN 10000
  char buffer[convert_UCIMLDB_BUF_LEN];
  char *cp=buffer;
  char *word=buffer;
  char *cp2;
  real *p;
  int line_len;

  if (!f)
    PLERROR("In loadUCIMLDB, could not open file %s for reading",filename.c_str());

  if((to_symbols && !to_n_symbols) || (!to_symbols && to_n_symbols))
    PLERROR("In loadUCIMLDB, to_symbols and to_nsymbols must both be provided (non-null), or both be 0");

  /*  first figure out number of columns and number of rows  */
  
  while (!feof(f))
  {
    fgets(buffer,convert_UCIMLDB_BUF_LEN,f);
    if (n_rows == -1) {
      /*  read number of columns  */
      while ((cp=strchr(cp,',')))
      {
        cp++;
        n_cols++;
      }
    }
    n_rows++;
  }

  fclose(f);

  /*  figure out the set of symbols used for each symbolic row, if any  */
  symbols = (char ***)calloc(n_cols,sizeof(char **));
  n_symbols = (int *)calloc(n_cols,sizeof(int));
  if(to_symbols)
  {
    *to_symbols = symbols;
    *to_n_symbols = n_symbols;
  }
  f = fopen(filename.c_str(),"r");
  for (i=0;i<n_rows;i++)
  {
    fgets(buffer,convert_UCIMLDB_BUF_LEN,f);
    line_len=strlen(buffer);
    cp=word=buffer;
    for (j=0;j<n_cols;j++)
    {
      /*  find next end of word  */
      while ((*cp!=',' && *cp!='\n') && cp<=buffer+line_len) cp++;
      *cp=0;
      /*  is this symbolic?  */
      cp2=word;
      while (!isalpha((int)*cp2) && *cp2!='?' && cp2 < cp) cp2++;
      if (isalpha((int)*cp2) && *cp2!='?') { 
        /*  yes, non-misisng symbolic character was found:  */
        if (symbols[j])
        { 
          /*  we already had found symbols in this column  */
          int w=0;
          while (symbols[j][w] &&  /*  look for this symbol  */
                 strcmp(symbols[j][w],word)!=0 &&
                 w<n_symbols[j]) w++;
          if (w==n_rows)
            PLERROR("logic error in loadUCIMLDB");
          if (!symbols[j][w])
          {
            /*  new symbol  */
            symbols[j][w] = (char *)calloc(strlen(word)+1,sizeof(char));
            strcpy(symbols[j][w],word);
            n_symbols[j]++;
          }
        }
        else
        {
          /*  it's the first time we find a symbol in this column  */
          symbols[j] = (char **)calloc(n_rows,sizeof(char *));
          symbols[j][0] = (char *)calloc(strlen(word)+1,sizeof(char));
          strcpy(symbols[j][0], word);
          n_symbols[j]=1;
        }
      }
      word = cp+1;
    }
  }
  fclose(f);

  /*  sort the symbols  */
  for (j=0;j<n_cols;j++)
    if (symbols[j]) /*  it has symbols  */
      qsort(symbols[j],n_symbols[j],sizeof(char *),compare_string_pointers);
      
  Mat mat(n_rows,n_cols);
  /*  NOW actually READ the data  */
  {
    p = mat.data();
    f = fopen(filename.c_str(),"r");
    for (i=0;i<n_rows;i++)
    {
      /*  read a row  */
      fgets(buffer,convert_UCIMLDB_BUF_LEN,f);
      line_len=strlen(buffer);
      cp=word=buffer;
      /*  interpret a row  */
      for (j=0;j<n_cols;j++)
      {
        /*  find end of word  */
        while ((*cp!=',' && *cp!='\n') && cp<=buffer+line_len) cp++;
        *cp=0;
        /*  is this missing?  */
        if (*word == '?')
          *p = MISSING_VALUE;
        else
          /*  is this symbolic?  */
          if (symbols[j]) {
            /*  read symbolic data  */
            int w=0;
            while (symbols[j][w] && /*  look for this symbol  */
                   strcmp(symbols[j][w],word)!=0 &&
                   w<n_symbols[j]) w++;
            if (w==n_rows || !symbols[j][w])
              PLERROR("logic error in loadUCIMLDB");
            *p = w;
          }
          else
          {
            /*  read numeric data  */
#ifdef USEDOUBLE
            sscanf(word,"%lf",p);
#else
            sscanf(word,"%f",p);
#endif
          }
        word=cp+1;
        p++;
      }
    }
    fclose(f);
  }

  if(!to_symbols)
  {
    for (int i=0; i<mat.width(); i++) 
    {
      for (int j=0; j<n_symbols[i]; j++)
        free(symbols[i][j]);
      free(symbols[i]);
    }
    free(symbols);
    free(n_symbols);
  }
#undef convert_UCIMLDB_BUF_LEN

  return mat;
}


// STATLOG machine-learning-database format
Mat loadSTATLOG(const string& filename, char ****to_symbols, int **to_n_symbols) 
{
  FILE *f = fopen(filename.c_str(),"r");
  int n_rows= -1, n_cols=0, i,j;
  char ***symbols;
  int *n_symbols;
#define convert_STATLOG_BUF_LEN 10000
  char buffer[convert_STATLOG_BUF_LEN];
  char *cp=buffer;
  char *word=buffer;
  char *cp2;
  real *p;
  int line_len;
  
  if (!f) 
    PLERROR("In loadSTATLOG, could not open file %s for reading",filename.c_str());
  
  if((to_symbols && !to_n_symbols) || (!to_symbols && to_n_symbols))
    PLERROR("In loadUCIMLDB, to_symbols and to_nsymbols must both be provided (non-null), or both be 0");

  /*  first figure out number of columns and number of rows  */
   
  while (!feof(f))
  {
    fgets(buffer,convert_STATLOG_BUF_LEN,f);
    if (n_rows == -1)
    {
      /*  read number of columns  */
      while (*cp == ' ')  
        cp++;   /*  jumping over blancs at the start of a new line  */
      while ( *cp!=0 && *cp!='\n' )
      {
        while ( *cp != 0 && *cp != '\n' && *cp != ' ')
        {
          cp++; /*  read one colomn  */
        }
        n_cols++;
        while ( *cp != 0 && *cp != '\n' && *cp == ' ')
        {
          cp++;   /*  jumping over blancs separating columns  */  
        }              
      }
    }
    n_rows++;
  }
  fclose(f);


  /*  figure out the set of symbols used for each symbolic row, if any  */
  symbols = (char ***)calloc(n_cols,sizeof(char **));
  n_symbols = (int *)calloc(n_cols,sizeof(int));
  if (to_symbols)
  {
    *to_symbols = symbols;
    *to_n_symbols = n_symbols;
  }
  f = fopen(filename.c_str(),"r");
  for (i=0;i<n_rows;i++)
  {
    fgets(buffer,convert_STATLOG_BUF_LEN,f);
    line_len=strlen(buffer);
    cp=word=buffer;
    for (j=0;j<n_cols;j++)
    {
      /*  jumping over blancs at the start of a new line  */ 
      while (*cp == ' ')
      {
        cp++;   
        word++;
      } 
      /*  find next end of word  */
      while ((*cp!=' ' && *cp!='\n') && cp<=buffer+line_len) cp++;
      *cp=0;
      /*  is this symbolic?  */
      cp2=word;
      while (!isalpha((int)*cp2) && *cp2!='?' && cp2 < cp) cp2++;
      if (isalpha((int)*cp2) && *cp2!='?')
      { 
        /*  yes, non-misisng symbolic character was found:  */
        if (symbols[j]) { 
          /*  we already had found symbols in this column  */
          int w=0;
          while (symbols[j][w] &&  /*  look for this symbol  */
                 strcmp(symbols[j][w],word)!=0 &&
                 w<n_symbols[j]) w++;
          if (w==n_rows)
            PLERROR("logic error in loadSTATLOG");
          if (!symbols[j][w])
          {
            /*  new symbol  */
            symbols[j][w] = (char *)calloc(strlen(word)+1,sizeof(char));
            strcpy(symbols[j][w],word);
            n_symbols[j]++;
          }
        }
        else
        {
          /*  it's the first time we find a symbol in this column  */
          symbols[j] = (char **)calloc(n_rows,sizeof(char *));
          symbols[j][0] = (char *)calloc(strlen(word)+1,sizeof(char));
          strcpy(symbols[j][0], word);
          n_symbols[j]=1;
        }
      }
      word = cp+1;
    }
  }
  fclose(f);

  /*  sort the symbols  */
  for (j=0;j<n_cols;j++)
    if (symbols[j]) /*  it has symbols  */
      qsort(symbols[j],n_symbols[j],sizeof(char *),compare_string_pointers);

  Mat mat(n_rows, n_cols);
  /*  NOW actually READ the data  */
  {
    p = mat.data();
    f = fopen(filename.c_str(),"r");
    for (i=0;i<n_rows;i++)
    {
      /*  read a row  */
      fgets(buffer,convert_STATLOG_BUF_LEN,f);
      line_len=strlen(buffer);
      cp=word=buffer;
      /*  interpret a row  */
      for (j=0;j<n_cols;j++)
      {
        /*  jumping over blancs at the start of a new line  */ 
        while (*cp == ' ')
        {
          cp++;   
          word++;
        } 
        /*  find end of word  */
        while ((*cp!=' ' && *cp!='\n') && cp<=buffer+line_len) cp++;
        *cp=0;
        /*  is this missing?  */
        if (*word == '?')
          *p = MISSING_VALUE;
        else
          /*  is this symbolic?  */
          if (symbols[j]) {
            /*  read symbolic data  */
            int w=0;
            while (symbols[j][w] && /*  look for this symbol  */
                   strcmp(symbols[j][w],word)!=0 &&
                   w<n_symbols[j]) w++;
            if (w==n_rows || !symbols[j][w])
              PLERROR("logic error in loadSTATLOG");
            *p = w;
          }
          else
          {
            /*  read numeric data  */
#ifdef USEDOUBLE
            sscanf(word,"%lf",p);
#else
            sscanf(word,"%f",p);
#endif
          }
        word=cp+1;
        p++;
      }
    }
    fclose(f);
  }

  if(!to_symbols)
  {
    for (int i=0; i<mat.width(); i++) 
    {
      for (int j=0; j<n_symbols[i]; j++)
        free(symbols[i][j]);
      free(symbols[i]);
    }
    free(symbols);
    free(n_symbols);
  }
#undef convert_STATLOG_BUF_LEN

  return mat;
}


// jpeg stuff...
void loadJPEGrgb(const string& jpeg_filename, Mat& rgbmat, int& row_size, int scale)
{
  string tmpfile = jpeg_filename + ".pnm";
  char command[1000];
  sprintf(command,"djpeg -pnm -scale 1/%d %s > %s",
          scale,jpeg_filename.c_str(),tmpfile.c_str());
  system(command);
  FILE* fp = fopen(tmpfile.c_str(),"r");
  if (!fp)
    PLERROR("reading %s",tmpfile.c_str());
  fscanf(fp,"%s",command);
  int w,h;
  fscanf(fp,"%d %d\n",&w,&h);
  fscanf(fp,"%*d\n");
  int n=w*h;
  rgbmat.resize(n,3);
  real *d=rgbmat.data();
  for (int i=0;i<n;i++)
    for (int k=0;k<3;k++,d++)
      *d =(real)(getc(fp));
  fclose(fp);
  sprintf(command,"rm %s",tmpfile.c_str());
  system(command);
  row_size = w;
}

} // end of namespace PLearn
