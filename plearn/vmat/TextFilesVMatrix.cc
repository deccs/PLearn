
// -*- C++ -*-

// TextFilesVMatrix.h
//
// Copyright (C) 2003-2004 ApSTAT Technologies Inc.
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

/* *******************************************************      
 * $Id$ 
 ******************************************************* */

// Author: Pascal Vincent

/*! \file TextFilesVMatrix.cc */
#include <plearn/base/PDate.h>
#include <plearn/base/ProgressBar.h>
#include <plearn/base/stringutils.h>
#include <plearn/io/load_and_save.h>
#include "TextFilesVMatrix.h"

namespace PLearn {
using namespace std;


char TextFilesVMatrix::buf[50000];

TextFilesVMatrix::TextFilesVMatrix()
    : idxfile(0),
      delimiter("\t"),
      auto_build_map(false),
      auto_extend_map(true),
      build_vmatrix_stringmap(false)
{
}

PLEARN_IMPLEMENT_OBJECT(
    TextFilesVMatrix,
    "Parse and represent a text file as a VMatrix",
    "This VMatrix contains a plethora of options for parsing text files,\n"
    "interpreting the fields (including arbitrary string fields), and\n"
    "representing the result as a numerical VMatrix.  It can be used to parse\n"
    "both SAS and CSV files.\n"
    "\n"
    "The metadatadir option should probably be specified.\n"
    "\n"
    "Internally, the metadata directory contains the following files:\n"
    " - a txtmat.idx binary index file (which will be automatically rebuilt if any of the raw text files is newer)\n"
    " - a txtmat.idx.log file reporting problems encountered while building the .idx file\n"
    "\n"
    "The txtmat.idx file is a binary file structured as follows\n"
    "- 1 byte indicating endianness: 'L' or 'B'\n"
    "- 4 byte int for length (number of data rows in the raw text file)\n"
    "- (unsigned char fileno, int pos) indicating in which raw text file and at what position each row starts\n"
    );


void TextFilesVMatrix::getFileAndPos(int i, unsigned char& fileno, int& pos) const
{
    if(i<0 || i>=length())
        PLERROR("TextFilesVMatrix::getFileAndPos out of range row %d (only %d rows)", i, length());
    fseek(idxfile, 5+i*5, SEEK_SET);
    fileno = fgetc(idxfile);
    fread(&pos, sizeof(int), 1, idxfile);
}

int TextFilesVMatrix::getIndexOfTextField(const string& fieldname) const
{
    int n = fieldspec.size();
    for(int i=0; i<n; i++)
        if(fieldspec[i].first==fieldname)
            return i;
    PLERROR("In TextFilesVMatrix::getIndexOfTextField unknown field %s",fieldname.c_str()); 
    return -1; // to make the compiler happy
}

void TextFilesVMatrix::buildIdx()
{
    cerr << "Building the index file. PLease be patient..." << endl;

    if(idxfile)
        fclose(idxfile);

    idxfile = fopen(( getMetaDataDir()/"txtmat.idx").c_str(),"wb");
    FILE* logfile = fopen((getMetaDataDir()/"txtmat.idx.log").c_str(),"a");

    if (! idxfile)
        PLERROR("TextFilesVMatrix::buildIdx: could not open index file '%s'",
                ( getMetaDataDir()/"txtmat.idx").c_str());
    if (! logfile)
        PLERROR("TextFilesVMatrix::buildIdx: could not open log file '%s'",
                (getMetaDataDir()/"txtmat.idx.log").c_str());

    // write endianness
    fputc(byte_order(), idxfile);
    // We don't know length yet, 
    length_ = 0;
    fwrite(&length_, 4, 1, idxfile);

    TVec<string> fields;

    int lineno = 0;
    for(unsigned char fileno=0; fileno<txtfiles.length(); fileno++)
    {
        FILE* f = txtfiles[fileno];
        fseek(f,0,SEEK_SET);

        int nskip = 0; // number of header lines to skip
        if(!skipheader.isEmpty())
            nskip = skipheader[fileno];

        // read the data rows and build the index
        for(;;)
        {
            long pos_long = ftell(f);
            if (pos_long > INT_MAX)
                PLERROR("In TextFilesVMatrix::buildIdx - 'pos_long' cannot be "
                        "more than %d", INT_MAX);
            int pos = int(pos_long);
            if(!fgets(buf, sizeof(buf), f))
                break;
            buf[sizeof(buf)-1] = '\0';         // ensure null-terminated
            lineno++;
            if(nskip>0)
                --nskip;
            else if(!isBlank(buf))
            {
                fields = splitIntoFields(buf);
                int nf = fields.length();
                if(nf!=fieldspec.size())
                    fprintf(logfile, "ERROR In file %d line %d: Found %d fields (should be %d):\n %s",fileno,lineno,nf,fieldspec.size(),buf);
                else  // Row OK! append it to index
                {
                    fputc(fileno, idxfile);
                    fwrite(&pos, 4, 1, idxfile);
                    length_++;
                }
            }
        } // end of loop over lines of file      
    } // end of loop over files

    // Write true length and width
    fseek(idxfile, 1, SEEK_SET);
    fwrite(&length_, 4, 1, idxfile);

    // close files
    fclose(logfile);
    fclose(idxfile);

    cerr << "Index file built." << endl;
}

/////////////////////////////
// isValidNonSkipFieldType //
/////////////////////////////
bool TextFilesVMatrix::isValidNonSkipFieldType(const string& ftype) const {
    return (ftype=="auto" || ftype=="date" || ftype=="jdate" || ftype=="postal" ||
            ftype=="dollar" || ftype=="YYYYMM" || ftype=="sas_date" || ftype == "bell_range");
}

void TextFilesVMatrix::setColumnNamesAndWidth()
{
    width_ = 0;
    TVec<string> fnames;
    for(int k=0; k<fieldspec.length(); k++)
    {
        string fname = fieldspec[k].first;
        string ftype = fieldspec[k].second;
        if(isValidNonSkipFieldType(ftype))
        {
            // declare the column name 
            fnames.push_back(fname);
            colrange.push_back( pair<int,int>(width_,1) );
            ++width_;
        }
        else if(ftype=="skip")
        {
            colrange.push_back( pair<int,int>(width_,0) );          
        }
        else
            PLERROR("In TextFilesVMatrix::setColumnNamesAndWidth, Invalid field type specification for field %s: %s",fname.c_str(), ftype.c_str());
    }
    for(int j=0; j<width_; j++)
        declareField(j, fnames[j]);
}

void TextFilesVMatrix::build_()
{
    // Initialize some sizespp
    int n = fieldspec.size();
    mapping.resize(n);
    mapfiles.resize(n);
    mapfiles.fill(0);

    setMetaDataDir(metadatapath);              // should be changed for metadatadir!?!?

    if(!force_mkdir(getMetaDataDir()))
        PLERROR("In TextFilesVMatrix::build_: could not create directory '%s'",
                getMetaDataDir().absolute().c_str());
    PPath metadir = getMetaDataDir();
    PPath idxfname = metadir/"txtmat.idx";

    setColumnNamesAndWidth();

    // Now open txtfiles
    int nf = txtfilenames.length();
    txtfiles.resize(nf);
    for(int k=0; k<nf; k++)
    {
        string fnam = txtfilenames[k];
        txtfiles[k] = fopen(fnam.c_str(),"r");
    }

    // open the index file
    if(!isfile(idxfname))
        buildIdx(); // (re)build it first!
    idxfile = fopen(idxfname.c_str(),"rb");
    if(fgetc(idxfile) != byte_order())
        PLERROR("Wrong endianness. Remove the index file for it to be automatically rebuilt");
    fread(&length_, 4, 1, idxfile);

    // Handle string mapping
    loadMappings();

    if (auto_build_map)
        autoBuildMappings();

    if(build_vmatrix_stringmap)
        buildVMatrixStringMapping();

    // Sanity checking
    if (delimiter.size() != 1)
        PLERROR("TextFilesVMatrix: the 'delimiter' option '%s' must contain exactly one character",
                delimiter.c_str());
}


string TextFilesVMatrix::getTextRow(int i) const
{
    unsigned char fileno;
    int pos;
    getFileAndPos(i, fileno, pos);
    FILE* f = txtfiles[fileno];
    fseek(f,pos,SEEK_SET);
    if(!fgets(buf, sizeof(buf), f))
        PLERROR("Problem in TextFilesVMatrix::getTextRow fgets for row %d returned NULL",i);
    return removenewline(buf);
}

void TextFilesVMatrix::loadMappings()
{
//  static char buf[1000];
    int n = fieldspec.size();
    for(int k=0; k<n; k++)
    {
        string fname = getMapFilePath(k);
        if (isfile(fname)) {
            vector<string> all_lines = getNonBlankLines(loadFileAsString(fname));
            for (size_t i = 0; i < all_lines.size(); i++) {
                string map_line = all_lines[i];
                size_t start_of_string = map_line.find('"');
                size_t end_of_string = map_line.rfind('"');
                string strval = map_line.substr(start_of_string + 1, end_of_string - start_of_string - 1);
                string real_val_str = map_line.substr(end_of_string + 1);
                real real_val;
                if (!pl_isnumber(real_val_str, &real_val))
                    PLERROR("In TextFilesVMatrix::loadMappings - Found a mapping to something that is not a number");
                mapping[k][strval] = real_val;
            }
        }

        /*
          FILE* f = fopen(fname.c_str(),"rb");
          if(f)
          {
          int c = 0;
          while(c!=EOF)
          {
          int l =0;
          do { c = fgetc(f); }
          while(c!='"' && c!=EOF);
          if(c==EOF)
          break;
          do { c = fgetc(f); buf[l++]=c; }
          while(c!='"' && c!=EOF);
          buf[--l] = '\0';
          string strval = buf;
          real val;
          fscanf(f,"%lf",&val);
          mapping[k][strval] = val;
          }
          fclose(f);
          }
        */
    }
}

///////////////////////
// autoBuildMappings //
///////////////////////
void TextFilesVMatrix::autoBuildMappings() {
    // TODO We should somehow check the date of existing mappings to see if they need to be built.
    // For now we just create them if they do not exist yet.

    // First make sure there is no existing mapping.
    bool already_exist = false;
    for (int i = 0; !already_exist && i < mapping.length(); i++) {
        if (!mapping[i].empty())
            already_exist = true;
    }
    if (!already_exist) {
        // Mappings need to be built.
        // We do this by reading the whole data.
        Vec row(width());
        bool auto_extend_map_backup = auto_extend_map;
        auto_extend_map = true;
        ProgressBar pb("Building mappings", length());
        for (int i = 0; i < length(); i++) {
            getRow(i, row);
            pb.update(i + 1);
        }
        auto_extend_map = auto_extend_map_backup;
    }
}

void TextFilesVMatrix::generateMapCounts()
{
    int n = fieldspec.size();
    TVec< hash_map<string, int> > counts(n);
    for(int k=0; k<n; k++)
    {
        if(!mapping[k].empty())
        {
            hash_map<string, real>& mapping_k = mapping[k];
            hash_map<string, int>& counts_k = counts[k];          
            hash_map<string, real>::const_iterator it = mapping_k.begin();
            hash_map<string, real>::const_iterator itend = mapping_k.end();
            while(it!=itend)
            {
                counts_k[ it->first ] = 0;
                ++it;
            }
        }
    }

    int l = length();
    ProgressBar pg("Generating counts of mappings",l);
    for(int i=0; i<l; i++)
    {
        TVec<string> fields = getTextFields(i);
        for(int k=0; k<fields.length(); k++)
        {
            if(mapping[k].find(fields[k])!=mapping[k].end())
                ++counts[k][fields[k]];
        }
        pg(i);
    }
  
    // Save the counts
    for(int k=0; k<n; k++)
    {
        if(!counts[k].empty())
            PLearn::save( getMetaDataDir() / "counts" / fieldspec[k].first+".count", counts[k] );
    }
  
}

void TextFilesVMatrix::buildVMatrixStringMapping()
{
    int n = fieldspec.size();
    for(int k=0; k<n; k++)
    {
        if(mapping[k].size()>0)
        {
            // get the corresponding VMatrix column range and add the VMatrix mapping
            int colstart = colrange[k].first;
            int ncols = colrange[k].second;
            hash_map<string,real>::const_iterator it = mapping[k].begin();
            hash_map<string,real>::const_iterator itend = mapping[k].end();
            while(it!=itend)
            {              
                for(int j=colstart; j<colstart+ncols; j++)
                    addStringMapping(j, it->first, it->second);
                ++it;
            }
        }
    }
}

real TextFilesVMatrix::getMapping(int fieldnum, const string& strval) const
{
    hash_map<string, real>& m = mapping[fieldnum];
    hash_map<string, real>::const_iterator found = m.find(strval);
    if(found!=m.end()) // found it!
        return found->second;

    // strval not found
    if(!auto_extend_map)
        PLERROR("No mapping found for field %d (%s) string-value \"%s\" ", fieldnum, fieldspec[fieldnum].first.c_str(), strval.c_str());

    // OK, let's extend the mapping...
    real val = real(-1000 - int(m.size()));
    m[strval] = val;

    if(!mapfiles[fieldnum])
    {
        string fname = getMapFilePath(fieldnum);
        force_mkdir_for_file(fname);
        mapfiles[fieldnum] = fopen(fname.c_str(),"a");
        if(!mapfiles[fieldnum])
            PLERROR("Could not open map file %s\n for appending\n",fname.c_str());
    }
    
    fprintf(mapfiles[fieldnum],"\n\"%s\" %f", strval.c_str(), val);
    return val;
}

TVec<string> TextFilesVMatrix::splitIntoFields(const string& raw_row) const
{
    return split(raw_row, delimiter[0]);
}

TVec<string> TextFilesVMatrix::getTextFields(int i) const
{
    string rowi = getTextRow(i);  
    TVec<string> fields =  splitIntoFields(rowi);
    if(fields.size() != fieldspec.size())
        PLERROR("In getting fields of row %d, wrong number of fields: %d (should be %d):\n%s\n",i,fields.size(),fieldspec.size(),rowi.c_str());
    for(int k=0; k<fields.size(); k++)
        fields[k] = removeblanks(fields[k]);
    return fields;
}

real TextFilesVMatrix::getPostalEncoding(const string& strval, bool display_warning) const
{
    if(strval=="")
        return MISSING_VALUE;
  
    char first_char = strval[0];
    int second_digit = strval[1];
    real val = 0;
    if(first_char=='A')
        val = 30 + second_digit;
    else if(first_char=='B')
        val = 40 + second_digit;
    else if(first_char=='C')
        val = 50 + second_digit;
    else if(first_char=='E')
        val = 60 + second_digit;
    else if(first_char=='G')
        val = 0 + second_digit;
    else if(first_char=='H')
        val = 10 + second_digit;
    else if(first_char=='J')
        val = 20 + second_digit;
    else if(first_char=='K')
        val = 70 + second_digit;
    else if(first_char=='L')
        val = 80 + second_digit;
    else if(first_char=='M')
        val = 90 + second_digit;
    else if(first_char=='N')
        val = 100 + second_digit;
    else if(first_char=='P')
        val = 110 + second_digit;
    else if(first_char=='R')
        val = 120 + second_digit;
    else if(first_char=='S')
        val = 130 + second_digit;
    else if(first_char=='T')
        val = 140 + second_digit;
    else if(first_char=='V')
        val = 150 + second_digit;
    else if(first_char=='W')
        val = 160 + second_digit;
    else if(first_char=='X')
        val = 170 + second_digit;
    else if(first_char=='0' || first_char=='1' || first_char=='2' || first_char=='3' ||
            first_char=='4' || first_char=='5' || first_char=='6' || first_char=='7' ||
            first_char=='8' || first_char=='9') {
        // That would be a C.P.
        int first_digit = strval[0];
        val = 260 + first_digit * 10 + second_digit;
    }
    else {
        if (display_warning) {
            string errmsg = "Currently only some postal codes are supported: ";
            errmsg += "can't process " + strval + ", value will be set to 0.";
            PLWARNING(errmsg.c_str());
        }
        val = 0;
    }

    return val;
}

void TextFilesVMatrix::transformStringToValue(int k, string strval, Vec dest) const
{
    strval = removeblanks(strval);
    string fieldname = fieldspec[k].first;
    string fieldtype = fieldspec[k].second;
    real val;

    if(dest.length() != colrange[k].second)
        PLERROR("In TextFilesVMatrix::transformStringToValue, destination vec for field %d should be of length %d, not %d",k,colrange[k].second, dest.length());


    if(fieldtype=="skip")
    {
        // do nothing, simply skip it
    }
    else if(fieldtype=="auto")
    {
        if(strval=="")  // missing
            dest[0] = MISSING_VALUE;
        else if(pl_isnumber(strval,&val))
            dest[0] = real(val);
        else
            dest[0] = getMapping(k, strval);
    }
    else if(fieldtype=="date")
    {
        if(strval=="")  // missing
            dest[0] = MISSING_VALUE;
        else
            dest[0] = date_to_float(PDate(strval));
    }

    else if(fieldtype=="jdate")
    {
        if(strval=="")  // missing
            dest[0] = MISSING_VALUE;
        else
            dest[0] = PDate(strval).toJulianDay();
    }

    else if(fieldtype=="sas_date")
    {
        if(strval=="" || strval == "0")  // missing
            dest[0] = MISSING_VALUE;
        else if(pl_isnumber(strval,&val)) {
            dest[0] = val;
            if (val <= 0) {
                PLERROR("I didn't know a sas_date could be negative");
            }
        }
        else
            PLERROR("Error while parsing a sas_date");
    }

    else if(fieldtype=="YYYYMM")
    {
        if(strval=="" || !pl_isnumber(strval) || toint(strval)<197000)
            dest[0] = MISSING_VALUE;
        else
            dest[0] = PDate(strval+"01").toJulianDay();
    }

    else if(fieldtype=="postal")
    {
        dest[0] = getPostalEncoding(strval);
    }
    else if(fieldtype=="dollar")
    {
        if(strval=="")  // missing
            dest[0] = MISSING_VALUE;
        else if(strval[0]=='$')
        {
            string s = "";
            for(unsigned int pos=1; pos<strval.size(); pos++)
                if(!isspace(strval[pos]))
                    s += strval[pos];
              
            if(pl_isnumber(s,&val))
                dest[0] = real(val);
            else
                dest[0] = getMapping(k, strval);
        }
        else
            dest[0] = getMapping(k, strval);
    }
    else if(fieldtype=="bell_range") {
        if (strval == "") {
            // Missing value.
            dest[0] = MISSING_VALUE;
        } else if (strval == "Negative Value") {
            // We put an arbitrary negative value since we don't have more info.
            dest[0] = -100;
        } else {
            // A range of the kind "A: $0- 250".
            string s = "";
            unsigned int pos;
            unsigned int end;
            for (pos=0; pos<strval.size() && (strval[pos] == ' ' || !pl_isnumber(strval.substr(pos,1))); pos++) {}
            for (end=pos; end<strval.size() && strval[end] != ' ' && pl_isnumber(strval.substr(end,1)); end++) {
                s += strval[end];
            }
            real number_1,number_2;
            if (!pl_isnumber(s,&number_1) || is_missing(number_1)) {
                PLERROR(("TextFilesVMatrix::transformStringToValue: " + strval +
                         " is not a well formatted Bell range").c_str());
            }
            s = "";
            for (pos=end; pos<strval.size() && (strval[pos] == ' ' || !pl_isnumber(strval.substr(pos,1))); pos++) {}
            for (end=pos; end<strval.size() && strval[end] != ' ' && pl_isnumber(strval.substr(end,1)); end++) {
                s += strval[end];
            }
            if (!pl_isnumber(s,&number_2) || is_missing(number_2)) {
                PLERROR(("TextFilesVMatrix::transformStringToValue: " + strval +
                         " is not a well formatted Bell range").c_str());
            }
            dest[0] = (number_1 + number_2) / (real) 2;
        }
    }

    else
    {
        PLERROR("TextFilesVMatrix::TextFilesVMatrix::transformStringToValue, Invalid field type specification for field %s: %s",fieldname.c_str(), fieldtype.c_str());
    }
}

void TextFilesVMatrix::getNewRow(int i, const Vec& v) const
{
    TVec<string> fields = getTextFields(i);
    int n = fields.size();

    for(int k=0; k<n; k++)
    {
        string fieldname = fieldspec[k].first;
        string fieldtype = fieldspec[k].second;
        string strval = fields[k];
        Vec dest = v.subVec(colrange[k].first, colrange[k].second);

        try 
        { transformStringToValue(k, strval, dest); }
        catch(const PLearnError& e)
        {
            PLERROR("In TextFilesVMatrix, while parsing field %d (%s) of row %d: \n%s",
                    k,fieldname.c_str(),i,e.message().c_str());
        }
    }
}

void TextFilesVMatrix::declareOptions(OptionList& ol)
{
    declareOption(ol, "metadatapath", &TextFilesVMatrix::metadatapath, OptionBase::buildoption,
                  "Path of the metadata directory (in which to store the index, ...)");

    declareOption(ol, "txtfilenames", &TextFilesVMatrix::txtfilenames, OptionBase::buildoption,
                  "A list of paths to raw text files containing the records");

    declareOption(ol, "delimiter", &TextFilesVMatrix::delimiter, OptionBase::buildoption,
                  "Delimiter to use to split the fields.  Common delimiters are:\n"
                  "- \"\\t\" : used for SAS files (the default)\n"
                  "- \",\"  : used for CSV files\n"
                  "- \";\"  : used for a variant of CSV files");
  
    declareOption(ol, "skipheader", &TextFilesVMatrix::skipheader, OptionBase::buildoption,
                  "An (optional) list of integers, one for each of the txtfilenames,\n"
                  "indicating the number of header lines at the top of the file to be skipped.");

    declareOption(ol, "fieldspec", &TextFilesVMatrix::fieldspec, OptionBase::buildoption,
                  "Specification of field names and types (type indicates how the text field is to be mapped to one or more reals)\n"
                  "Currently supported types: \n"
                  "- skip       : Ignore the content of the field, won't be inserted in the resulting VMat\n"
                  "- auto       : If a numeric value, keep it as is, if not, look it up in the mapping (possibly inserting a new mapping if it's not there) \n"
                  "- date       : date of the form 25DEC2003 or 25-dec-2003 or 2003/12/25 or 20031225, will be mapped to float date format 1031225\n"
                  "- jdate      : date of the form 25DEC2003 or 25-dec-2003 or 2003/12/25 or 20031225, will be mapped to *julian* date format\n"
                  "- sas_date   : date used by SAS = number of days since Jan. 1st, 1960 (with 0 = missing)\n"
                  "- YYYYMM     : date of the form YYYYMM (e.g: 200312), will be mapped to the julian date of the corresponding month. Everthing "\
                  "               other than a number or lower than 197000 is considered as nan\n"
                  "- postal     : canadian postal code \n"
                  "- dollar     : strangely formatted field with dollar amount. Format is sth like '$12 003'\n"
                  "- bell_range : a range like \"A: $0- 250\", replaced by the average of the two bounds;\n"
                  "               if the \"Negative Value\" string is found, it is replaced by -100\n"
        );

    declareOption(ol, "auto_extend_map", &TextFilesVMatrix::auto_extend_map, OptionBase::buildoption,
                  "If true, new strings for fields of type AUTO will automatically appended to the mapping (in the metadata/mappings/fieldname.map file)");

    declareOption(ol, "auto_build_map", &TextFilesVMatrix::auto_build_map, OptionBase::buildoption,
                  "If true, all mappings will be automatically computed at build time if they do not exist yet\n");

    declareOption(ol, "build_vmatrix_stringmap", &TextFilesVMatrix::build_vmatrix_stringmap,
                  OptionBase::buildoption,
                  "If true, standard vmatrix stringmap will be built from the txtmat specific stringmap");
   

    // Now call the parent class' declareOptions
    inherited::declareOptions(ol);
}

void TextFilesVMatrix::readAndCheckOptionName(PStream& in, const string& optionname)
{
    in.skipBlanksAndComments();
    in.readUntil(buf, sizeof(buf), "= ");
    string option = removeblanks(buf);
    in.skipBlanksAndComments();
    char eq = in.get();
    if(option!=optionname || eq!='=')
        PLERROR("Bad syntax in .txtmat file.\n" 
                "Expected option %s = ...\n"
                "Read %s %c\n", optionname.c_str(), option.c_str(), eq); 
}


// ### Nothing to add here, simply calls build_
void TextFilesVMatrix::build()
{
    inherited::build();
    build_();
}

TextFilesVMatrix::~TextFilesVMatrix()
{
    if(idxfile)
        fclose(idxfile);
    for(int k=0; k<txtfiles.length(); k++)
        fclose(txtfiles[k]);

    for(int k=0; k<fieldspec.size(); k++)
    {
        if(mapfiles[k])
            fclose(mapfiles[k]);
    }
}

void TextFilesVMatrix::makeDeepCopyFromShallowCopy(CopiesMap& copies)
{
    inherited::makeDeepCopyFromShallowCopy(copies);
}

} // end of namespace PLearn


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
