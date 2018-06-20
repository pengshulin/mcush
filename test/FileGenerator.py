# coding: utf8
__doc__ = 'utility to generate files'
__author__ = 'Peng Shulin <trees_peng@163.com>'
from pprint import pprint
from time import ctime
from subprocess import call

HEAD_NOTES = '''NOTES:
THIS FILE IS GENERATED AUTOMATICALLY
DO NOT MODIFY IT MANUALLY !!!'''

MAX_LINE_LEN = 78

class FileGenerator:
    def __init__( self, filename, name='' ):
        self.name = name
        self.filename = filename
        self.fp = open( self.filename, 'w+' )
        self.writeHead()

    #def writeNotes( self, notes ):
    #    self.writeComment( notes )
     
    def writeHead( self ):
        self.writeSeperator( 'BEGIN OF FILE' )
        self.writeComment( HEAD_NOTES )
        self.writeComment( 'Modified at %s'% ctime() )
        self.writeSeperator( '' )

    def writeTail( self ):
        self.writeSeperator( 'END OF FILE' )
 
    def writeLine( self, string='' ):
        if self.fp:
            self.fp.write( '%s\n'% string )

    def writeLines( self, lines ):
        for line in lines:
            self.writeLine( line )
 
    def close( self ):
        self.writeTail()
        self.fp.close()
        self.fp = None
 
    def writeComment( self, comment ):
        pass
    
    def writeSeperator( self, title ):
        pass

 
class PythonGenerator( FileGenerator ):
    def writeHead( self ): 
        self.writeComment( 'encoding:utf-8' )
        self.writeDocString( None, self.name.strip() )
        FileGenerator.writeHead( self )

    def writeComment( self, comment ):
        for line in comment.split('\n'):
            self.writeLine( '# %s'% line.strip() )
 
    def writeSeperator( self, title='' ):
        line = title.join( '  ' ) if title.strip() else ''
        while (len(line) + 2) <= MAX_LINE_LEN:
            line = line.join( '##' )
        if len(line) < MAX_LINE_LEN:
            line += '#'
        self.writeLine( line )

    def writeDocString( self, name, docstring ):
        if name:
            self.writeLine( '%s = \\'% name )
        self.writeLine( '\'\'\'%s\'\'\''% docstring )

    def writeObject( self, name, data ):
        self.writeLine( '%s = \\'%name )
        pprint( data, self.fp )
        self.writeLine( '\n' )



class CGenerator( FileGenerator ):
    def writeComment( self, comment ):
        for line in comment.split('\n'):
            while (len(line) + 6) < MAX_LINE_LEN:
                line += ' '
            self.writeLine( '/* %s */'% line )
 
    def writeSeperator( self, title='' ):
        line = title.join( '  ' ) if title.strip() else ''
        while (len(line) + 4) <= MAX_LINE_LEN:
            line = line.join( '**' )
        if (len(line) + 2) < MAX_LINE_LEN:
            line += '*'
        line = line.join( '//' )
        self.writeLine( line )

    def writeDocString( self, name, docstring ):
        if name:
            self.writeLine( 'char %s = \\'% name )
        self.writeLine( '"%s"'% docstring )

    def writeObject( self, name, data ):
        self.writeLine( '%s = \\'%name )
        pprint( data, self.fp )
        self.writeLine( '\n' )

    def writeHead( self ): 
        FileGenerator.writeHead( self )
        IN_C_DECLAR = self.name.strip().replace('.','_').upper()
        self.writeLine( '#define __IN_%s__'% IN_C_DECLAR )


class HGenerator( CGenerator ):
    def writeHead( self ): 
        FileGenerator.writeHead( self )
        INCLUDE_DEF = self.name.strip().replace('.','_').upper()
        self.writeLine( '#ifndef __%s__'%INCLUDE_DEF )
        self.writeLine( '#define __%s__'%INCLUDE_DEF )

    def writeTail( self ): 
        self.writeLine( '#endif' )
        FileGenerator.writeTail( self )


