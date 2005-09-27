// -*- C++ -*-

// PythonObjectWrapper.cc
//
// Copyright (C) 2005 Nicolas Chapados 
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
   * $Id: .pyskeleton_header 544 2003-09-01 00:05:31Z plearner $ 
   ******************************************************* */

// Authors: Nicolas Chapados

/*! \file PythonObjectWrapper.cc */

// Must include Python first...
#include "PythonObjectWrapper.h"

// From C/C++ stdlib
#include <stdio.h>
#include <algorithm>

// From PLearn
#include <plearn/base/plerror.h>

namespace PLearn {
using namespace std;

// Error-reporting
void PLPythonConversionError(const char* function_name,
                             PyObject* pyobj)
{
    fprintf(stderr,"For python object: ");
    PyObject_Print(pyobj, stderr, Py_PRINT_RAW);
    PLERROR("Cannot convert Python object using %s", function_name);
}

// Python initialization
void PythonObjectWrapper::initializePython()
{
    static bool numarray_initialized = false;
    if (! numarray_initialized) {
        // must be in each translation unit that makes use of libnumarray;
        // weird stuff related to table of function pointers that's being
        // initialized into a STATIC VARIABLE of the translation unit!
        import_libnumarray();
        numarray_initialized = true;
    }
}


//#####  ConvertFromPyObject  #################################################

int ConvertFromPyObject<int>::convert(PyObject* pyobj)
{
    assert( pyobj );
    if (! PyInt_Check(pyobj))
        PLPythonConversionError("ConvertFromPyObject<int>", pyobj);
    return int(PyInt_AS_LONG(pyobj));
}

long ConvertFromPyObject<long>::convert(PyObject* pyobj)
{
    assert( pyobj );
    if (! PyLong_Check(pyobj))
        PLPythonConversionError("ConvertFromPyObject<long>", pyobj);
    return PyLong_AsLong(pyobj);
}

double ConvertFromPyObject<double>::convert(PyObject* pyobj)
{
    assert( pyobj );
    if (! PyFloat_Check(pyobj))
        PLPythonConversionError("ConvertFromPyObject<double>", pyobj);
    return PyFloat_AS_DOUBLE(pyobj);
}

string ConvertFromPyObject<string>::convert(PyObject* pyobj)
{
    assert( pyobj );
    if (! PyString_Check(pyobj))
        PLPythonConversionError("ConvertFromPyObject<string>", pyobj);
    return PyString_AsString(pyobj);
}

void ConvertFromPyObject<Vec>::convert(PyObject* pyobj, Vec& v)
{
    // NA_InputArray possibly creates a well-behaved temporary (i.e. not
    // discontinuous is memory)
    assert( pyobj );
    PyArrayObject* pyarr = NA_InputArray(pyobj, tFloat64, NUM_C_ARRAY);
    if (! pyarr)
        PLPythonConversionError("ConvertFromPyObject<Vec>", pyobj);
    if (pyarr->nd != 1)
        PLERROR("ConvertFromPyObject<Vec>: Dimensionality of the returned array "
                "should be 1; got %d", pyarr->nd);

    v.resize(pyarr->dimensions[0]);
    v.copyFrom((double*)(NA_OFFSETDATA(pyarr)), pyarr->dimensions[0]);
    Py_XDECREF(pyarr);
}

Vec ConvertFromPyObject<Vec>::convert(PyObject* pyobj)
{
    Vec v;
    convert(pyobj, v);
    return v;
}

void ConvertFromPyObject<Mat>::convert(PyObject* pyobj, Mat& m)
{
    // NA_InputArray possibly creates a well-behaved temporary (i.e. not
    // discontinuous is memory)
    assert( pyobj );
    PyArrayObject* pyarr = NA_InputArray(pyobj, tFloat64, NUM_C_ARRAY);
    if (! pyarr)
        PLPythonConversionError("ConvertFromPyObject<Mat>", pyobj);
    if (pyarr->nd != 2)
        PLERROR("ConvertFromPyObject<Mat>: Dimensionality of the returned array "
                "should be 2; got %d", pyarr->nd);

    m.resize(pyarr->dimensions[0], pyarr->dimensions[1]);
    m.toVec().copyFrom((double*)(NA_OFFSETDATA(pyarr)),
                       pyarr->dimensions[0] * pyarr->dimensions[1]);
    Py_XDECREF(pyarr);
}

Mat ConvertFromPyObject<Mat>::convert(PyObject* pyobj)
{
    Mat m;
    convert(pyobj, m);
    return m;
}


//#####  Constructors+Destructors  ############################################

// Copy constructor: increment refcount if controlling ownership.
PythonObjectWrapper::PythonObjectWrapper(const PythonObjectWrapper& other)
    : m_ownership(other.m_ownership),
      m_object(other.m_object)
{
    if (m_ownership == control_ownership)
        Py_XINCREF(m_object);
}

// Destructor decrements refcount if controlling ownership
PythonObjectWrapper::~PythonObjectWrapper()
{
    if (m_ownership == control_ownership)
        Py_XDECREF(m_object);
}

// Assignment: let copy ctor and dtor take care of ownership
PythonObjectWrapper& PythonObjectWrapper::operator=(const PythonObjectWrapper& rhs)
{
    if (&rhs != this) {
        PythonObjectWrapper other(rhs);
        swap(other);
    }
    return *this;
}

// Swap *this with another instance
void PythonObjectWrapper::swap(PythonObjectWrapper& other)
{
    std::swap(this->m_ownership, other.m_ownership);
    std::swap(this->m_object,    other.m_object);
}

// Print out the Python object to stderr for debugging purposes
void PythonObjectWrapper::printDebug() const
{
    PyObject_Print(m_object, stderr, Py_PRINT_RAW);
}


//#####  newPyObject  #########################################################

PyObject* PythonObjectWrapper::newPyObject(const Vec& data)
{
    PyArrayObject* pyarr = 0;
    if (data.isNull() || data.isEmpty())
        pyarr = NA_NewArray(NULL, tFloat64, 1, 0);
    else
        pyarr = NA_NewArray(data.data(), tFloat64, 1, data.size());
        
    return (PyObject*)pyarr;
}

PyObject* PythonObjectWrapper::newPyObject(const Mat& data)
{
    PyArrayObject* pyarr = 0;
    if (data.isNull() || data.isEmpty())
        pyarr = NA_NewArray(NULL, tFloat64, 2, data.length(), data.width());
    else if (data.mod() == data.width())
        pyarr = NA_NewArray(data.data(), tFloat64, 2, data.length(), data.width());
    else
        PLERROR("PythonObjectWrapper::newPyObject: matrices with mod != width are not "
                "currently supported; we have width=%d / mod=%d", data.width(), data.mod());

    return (PyObject*)pyarr;
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
