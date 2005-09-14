# plearn_service.py
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

import os, string
from plearn.pyplearn import *
from plearn.pyplearn import plearn_repr
import plearn.plio
import sys

def launch_plearn_server(command = 'plearn server', logger=None):
    if logger: logger.info('LAUNCHING PLEARN SERVER: command = '+command)                
    to_server, from_server = os.popen2(command, 'b')
    return RemotePLearnServer(from_server, to_server, logger)
        
class RemotePLearnServer:

    def __init__(self, from_server, to_server, logger=None):
        """from_server and to_server are expected to be two file-like objects
        (supporting read, write, flush).
        If you wish to log debugging info, pass ad logger an instance of a logging.Logger
        as returned for ex. by logging.getLogger()
        """
        self.io = plearn.plio.PLearnIO(from_server, to_server)
        self.log = logger
        self.reserved_ids = []
        self.nextid = 1
        self.objects = {}
        self.clear_maps = True
        self.dbg_dump = False
        self.closed = False
        if self.log: self.log.info('CONNEXION ESTABLISHED WITH PLEARN SERVER')
        self.callFunction("binary")
        self.callFunction("implicit_storage",True)

    def reserve_new_id(self):
        """Returns an available object id and adds it to self.reserved_ids"""
        newid = 1
        if self.reserved_ids:
            first_id = self.reserved_ids[0]
            last_id  = self.reserved_ids[-1]
            if first_id>1:
                newid = first_id-1
                self.reserved_ids.insert(0,newid)
            elif last_id-first_id+1 == len(self.reserved_ids):
                newid = last_id+1
                self.reserved_ids.append(newid)
            else:
                prev_id = first_id
                for pos in xrange(1,len(self.reserved_ids)):
                    id = self.reserved_ids[pos]
                    if id-prev_id>1:
                        newid = id-1
                        self.reserved_ids.insert(pos,newid)
                        break
                    prev_id = id
        else:
            self.reserved_ids.append(newid)
        return newid
                
    def free_id(self,id):
        """Removes an object id from the list self.reserved_ids"""
        self.reserved_ids.remove(id)


    def new(self, objectspec):
        """
        objectspec is the specification of a plearn object.
        It can be either a string in the plearn serialization format,
        or a PyPLearnObject such as those built by calling pl.ClassName(...)
        """
        specstr = objectspec
        if type(specstr)!=str:
            specstr = specstr.plearn_repr()
            
        objid = self.reserve_new_id()        
        self.callNewObject(objid,specstr)
        obj = RemotePObject(self, objid)
        self.objects[objid] = obj
        return obj

    def load(self, objfilepath):
        objid = self.reserve_new_id()
        self.callLoadObject(objid, objfilepath)
        obj = RemotePObject(self, objid)
        self.objects[objid] = obj
        return obj

    def delete(self, obj):
        if type(obj) is int:
            objid = obj
        else:
            objid = obj.objid
        self.callDeleteObject(objid)
        del self.objects[objid]
        self.free_id(objid)

    def deleteAll(self):
        self.callDeleteAllObjects()
        del self.objects
        self.objects = {}
        self.reserved_ids = []

    def logged_write(self,msg):
        """Writes msg to self.io and possibly writes a corresponding entry in the logfile"""
        self.io.write(msg)
        if self.log: self.log.debug('SENDING: '+msg)
        
    def logged_write_args(self,args):
        argnum = 0
        for arg in args:
            self.io.write_typed(arg)
            if self.log: self.log.debug(' ARG_'+str(argnum)+': '+repr(arg))
            argnum += 1
        self.io.write('\n')
        self.io.flush()

    def callNewObject(self, objid, objspecstr):
        self.clearMaps()
        self.logged_write('!N '+str(objid)+' '+objspecstr+'\n')
        self.io.flush()
        self.expectResults(0)

    def callLoadObject(self, objid, filepath):
        self.clearMaps()
        self.logged_write('!L '+str(objid)+' '+filepath+'\n')
        self.io.flush()
        self.expectResults(0)

    def callDeleteObject(self, objid):
        self.logged_write('!D '+str(objid)+'\n')
        self.io.flush()
        self.expectResults(0)

    def callDeleteAllObjects(self):
        self.logged_write('!Z \n')
        self.io.flush()
        self.expectResults(0)

    def clearMaps(self):
        if self.clear_maps:
            self.io.clear_maps()

    def sendFunctionCallHeader(self, funcname, nargs):
        self.clearMaps()
        self.logged_write('!F '+funcname+' '+str(nargs)+' ')

    def sendMethodCallHeader(self, objid, methodname, nargs):
        self.clearMaps()
        self.logged_write('!M '+str(objid)+' '+methodname+' '+str(nargs)+' ')

    def getResultsCount(self):
        self.io.skip_blanks_and_comments()
        c = self.io.get()
        if c!='!':
            raise TypeError("Returns received from server are expected to start with a ! but read "+c)
        c = self.io.get()
        if c=='R':
            nreturned = self.io.read_int()
            if self.log: self.log.debug("RECEIVED RESULT COUNT: !"+c+' '+str(nreturned))
            return nreturned
        elif c=='E':
            msg = self.io.read_string()
            if self.log: self.log.error("RECEIVED ERROR: "+msg)                
            raise RuntimeError(msg)
        else:
            raise TypeError("Expected !R or !E but read !"+c)

    def expectResults(self, nresults_expected):
        # if self.log: self.log.debug("EXPECTING "+str(nresults_expected)+" RESULTS")
        nreturned = self.getResultsCount()
        if nreturned!=nresults_expected:
            raise TypeError("Expected "+str(nresults_expected)+" return arguments, but read R "+str(nreturned))

    def callFunction(self, functionname, *args):
        self.sendFunctionCallHeader(functionname, len(args))
        self.logged_write_args(args)
        self.io.flush()
        nresults = self.getResultsCount()
        results = []
        for i in xrange(nresults):
            res = self.io.binread()
            if self.log: self.log.debug(' RES_'+str(i)+': '+repr(res))
            results.append(res)
        if len(results)==1:
            return results[0]
        elif len(results)>1:
            return results

    def callMethod(self, objid, methodname, *args):
        self.sendMethodCallHeader(objid, methodname, len(args))
        self.logged_write_args(args)
        self.io.flush()

        if self.dbg_dump:
            print 'DEBUG DUMP AFTER CALL OF METHOD',methodname,args
            while True:
                sys.stderr.write(self.io.get())

        nresults = self.getResultsCount()
        # print 'Now reading',nresults,'results'
        #while True:
        #    print repr(self.io.readline())
        results = []
        for i in xrange(nresults):
            res = self.io.binread()
            if self.log: self.log.debug(' RES_'+str(i)+': '+repr(res))
            results.append(res)
        if len(results)==1:
            return results[0]
        elif len(results)>1:
            return results

    def close(self):
        if not self.closed:
            if self.log: self.log.info('NOW CLOSING: method close() called')                
            self.logged_write('!Q')
            self.io.flush()
            if self.log: self.log.info('WAITING FOR CHILD PROCESS TO FINISH: os.wait()')                
            os.wait()
            if self.log: self.log.info('CLOSED.')
            self.closed = True
                
    def __del__(self):
        self.close()
        
class RemotePObject:
    
    def __init__(self, serv, objid):
        self.server = serv
        self.objid = objid
        
    def __getattr__(self,methodname):
        def f(*args):
            return self.callMethod(methodname,*args)
        return f

    def callMethod(self,methodname, *args):
        return self.server.callMethod(self.objid, methodname, *args)

    def delete(self):
        self.server.delete(self.objid)

##     def getOptionAsString(self, optionname):
##         serv = self.server
##         serv.sendMethodCallHeader(self.objid, "getOptionAsString", 1)
##         serv.io.write_typed(optionname)
##         serv.io.flush()
##         serv.expectResults(1)
##         return serv.io.read_string()

##     def changeOptions(self, **options):
##         to_server.write()


