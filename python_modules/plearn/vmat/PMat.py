# PMat.py
# Copyright (C) 2005 Pascal Vincent
#
#  Redistribution and use in source and binary forms, with or without
#  modification, are permitted provided that the following conditions are met:
#
#   1. Redistributions of source code must retain the above copyright
#      notice, this list of conditions and the following disclaimer.
#
#   2. Redistributions in binary form must reproduce the above copyright
#      notice, this list of conditions and the following disclaimer in the
#      documentation and/or other materials provided with the distribution.
#
#   3. The name of the authors may not be used to endorse or promote
#      products derived from this software without specific prior written
#      permission.
#
#  THIS SOFTWARE IS PROVIDED BY THE AUTHORS ``AS IS'' AND ANY EXPRESS OR
#  IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
#  OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN
#  NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
#  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED
#  TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
#  PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
#  LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
#  NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
#  SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
#
#  This file is part of the PLearn library. For more information on the PLearn
#  library, go to the PLearn Web site at www.plearn.org


# Author: Pascal Vincent

import numarray, sys, os, os.path

def load_pmat_as_array(fname):
    s = file(fname,'rb').read()
    formatstr = s[0:64]
    datastr = s[64:]
    structuretype, l, w, data_type, endianness = formatstr.split()

    if data_type=='DOUBLE':
        elemtype = 'd'
    elif data_type=='FLOAT':
        elemtype = 'f'
    else:
        raise ValueError('Invalid data type in file header: '+data_type)

    if endianness=='LITTLE_ENDIAN':
        byteorder = 'little'
    elif endianness=='BIG_ENDIAN':
        byteorder = 'big'
    else:
        raise ValueError('Invalid endianness in file header: '+endianness)

    l = int(l)
    w = int(w)
    X = numarray.fromstring(datastr,elemtype, shape=(l,w) )
    if byteorder!=sys.byteorder:
        X.byteswap()
    return X


class PMat:

    def __init__(self, fname, openmode='r', fieldnames=[], elemtype='d',
                 inputsize=-1, targetsize=-1, weightsize=-1):
        self.fname = fname
        self.inputsize = inputsize
        self.targetsize = targetsize
        self.weightsize = weightsize
        if openmode=='r':
            self.f = open(fname,'rb')
            self.read_and_parse_header()
            self.load_fieldnames()            
                
        elif openmode=='w':
            self.f = open(fname,'w+b')
            self.fieldnames = fieldnames
            self.save_fieldnames()
            self.length = 0
            self.width = len(fieldnames)
            self.elemtype = elemtype
            self.swap_bytes = False
            self.write_header()
            
        elif openmode=='a':
            self.f = open(fname,'r+b')
            self.read_and_parse_header()
            self.load_fieldnames()

        else:
            raise ValueError("Currently only supported openmodes are 'r' and 'w': "+repr(openmode)+" is not supported")

    def write_header(self):
        header = 'MATRIX ' + str(self.length) + ' ' + str(self.width) + ' '

        if self.elemtype=='d':
            header += 'DOUBLE '
            self.elemsize = 8
        elif self.elemtype=='f':
            header += 'FLOAT '
            self.elemsize = 4
        else:
            raise TypeError('Unsupported elemtype: '+repr(elemtype))
        self.rowsize = self.elemsize*self.width

        if sys.byteorder=='little':
            header += 'LITTLE_ENDIAN '
        elif sys.byteorder=='big':
            header += 'BIG_ENDIAN '
        else:
            raise TypeError('Unsupported sys.byteorder: '+repr(sys.byteorder))
        
        header += ' '*(63-len(header))+'\n'

        self.f.seek(0)
        self.f.write(header)
        
    def read_and_parse_header(self):        
            header = self.f.read(64)
            mat_type, l, w, data_type, endianness = header.split()
            if mat_type!='MATRIX':
                raise ValueError('Invalid file header (should start with MATRIX)')
            self.length = int(l)
            self.width = int(w)
            if endianness=='LITTLE_ENDIAN':
                byteorder = 'little'
            elif endianness=='BIG_ENDIAN':
                byteorder = 'big'
            else:
                raise ValueError('Invalid endianness in file header: '+endianness)
            self.swap_bytes = (byteorder!=sys.byteorder)

            if data_type=='DOUBLE':
                self.elemtype = 'd'
                self.elemsize = 8
            elif data_type=='FLOAT':
                self.elemtype = 'f'
                self.elemsize = 4
            else:
                raise ValueError('Invalid data type in file header: '+data_type)
            self.rowsize = self.elemsize*self.width

    def load_fieldnames(self):
        self.fieldnames = []
        fieldnamefile = os.path.join(self.fname+'.metadata','fieldnames')
        if os.path.isfile(fieldnamefile):
            for row in open(fieldnamefile):
                row = row.split()
                if len(row)>0:
                    self.fieldnames.append(row[0])
        else:
            fieldnames = map(str,range(self.w))

    def save_fieldnames(self):
        metadatadir = self.fname+'.metadata'
        if not os.path.isdir(metadatadir):
            os.mkdir(metadatadir)
        fieldnamefile = os.path.join(metadatadir,'fieldnames')
        f = open(fieldnamefile,'wb')
        for name in self.fieldnames:
            f.write(name+'\t0\n')
        f.close()

    def getRow(self,i):
        if i<0 or i>=self.length:
            raise IndexError('PMat index out of range')
        self.f.seek(64+i*self.rowsize)
        data = self.f.read(self.rowsize)
        ar = numarray.fromstring(data, self.elemtype, (self.width,))
        if self.swap_bytes:
            ar.byteswap()
        return ar

    def getRows(self,i,l):
        if i<0 or l<0 or i+l>self.length:
            raise IndexError('PMat index out of range')
        self.f.seek(64+i*self.rowsize)
        data = self.f.read(l*self.rowsize)
        ar = numarray.fromstring(data, self.elemtype, (l,self.width))
        if self.swap_bytes:
            ar.byteswap()
        return ar

    def putRow(self,i,row):
        if i<0 or i>=self.length:
            raise IndexError('PMat index out of range')
        if len(row)!=self.width:
            raise TypeError('length of row ('+str(len(row))+ ') differs from matrix width ('+str(self.width)+')')
        if i<0 or i>=self.length:
            raise IndexError
        if self.swap_bytes: # must make a copy and swap bytes
            ar = numarray.numarray(row,type=self.elemtype)
            ar.byteswap()
        else: # asarray makes a copy if not already a numarray of the right type
            ar = numarray.asarray(row,type=self.elemtype)
        self.f.seek(64+i*self.rowsize)
        self.f.write(ar.tostring())

    def appendRow(self,row):
        if len(row)!=self.width:
            raise TypeError('length of row ('+str(len(row))+ ') differs from matrix width ('+str(self.width)+')')
        if self.swap_bytes: # must make a copy and swap bytes
            ar = numarray.numarray(row,type=self.elemtype)
            ar.byteswap()
        else: # asarray makes a copy if not already a numarray of the right type
            ar = numarray.asarray(row,type=self.elemtype)
        self.f.seek(64+self.length*self.rowsize)
        self.f.write(ar.tostring())
        self.length += 1
        self.write_header() # update length in header

    def flush(self):
        self.f.flush()

    def close(self):
        self.f.close()

    def append(self,row):
        self.appendRow(row)

    def __getitem__(self,i):
        if type(i)==slice:
            start, stop, step = i.start,i.stop,i.step
            if step!=None:
                raise IndexError('Extended slice with step not currently supported')
            l = self.length
            if stop>l:
                stop = l
            return self.getRows(start,stop-start)
        else:
            if i<0: i+=self.length
            return self.getRow(i)

    def __setitem__(self, i, row):
        l = self.length
        if i<0: i+=l
        self.putRow(i,row)

    def __len__(self):
        return self.length

    def plearn_repr(self):
        # asking for plearn_repr could be to send specification over
        # to another prg so that will open the .pmat
        # So we make sure data is flushed to disk.
        self.flush()
        
        res = 'FileVMatrix( filename="'+self.fname+'"\n' \
              + "inputsize = "+str(self.inputsize)+'\n' \
              + "targetsize = "+str(self.targetsize)+'\n' \
              + "weightsize = "+str(self.weightsize)+'\n' \
              + ") "
        return res
        
