from random import random

def GetMatrix( r, c, min = 0.0, max = 1.0 ):
    return [ [ random() * ( max - min ) + min for c in range( c ) ] for r in range(r) ]

def GetMatrixSq( n, min = 0.0, max = 1.0 ):
    return GetMatrix( n, n, min, max )

class MatrixStyle:
    def __init__(self):
        self.beforeMatrix = None
        self.matrixStart = None
        self.rowStart = "["
        self.firstValue = "{}"
        self.value = ", {}"
        self.rowEnd = "]"
        self.matrixEnd = None

class LinAlgCppMatrixStyle( MatrixStyle ):
    def __init__(self, numericType = "float", matrixName = "M", precision = 5 ):
        super().__init__()

        self.beforeMatrix = "LinearAlgebra::Matrix<{}> {}( ".format( numericType, matrixName )
        self.matrixStart = "{ \n"
        self.rowStart = "\t{ "

        if ( numericType == "float"):
            self.firstValue = "{{:.{}f}}f".format( precision )
            self.value = ", {{:.{}f}}f".format( precision )
        else:
            self.firstValue = "{{.{}f}}".format( precision )
            self.value= ", {{.{}f}}f".format( precision )
        
        self.rowEnd = " },\n"

        self.matrixEnd = " } );"

class LinAlgCppVectorStyle( MatrixStyle ):
    def __init__(self, numericType = "float", matrixName = "x", precision = 5 ):
        super().__init__()

        self.matrixStart = "LinearAlgebra::Vector<{}> {}( ".format( numericType, matrixName )
        self.rowStart = "{ "

        if ( numericType == "float"):
            self.firstValue = "{{:.{}f}}f".format( precision )
            self.value = ", {{:.{}f}}f".format( precision )
        else:
            self.firstValue = "{{.{}f}}".format( precision )
            self.value= ", {{.{}f}}f".format( precision )
        
        self.rowEnd = " } "

        self.matrixEnd = ");"


            

class MatrixFormatter:
    def __init__(self):
        pass

    def PrintPrintMatrix( self, m, style ):
        if style.beforeMatrix is not None:
            print( style.beforeMatrix, end = "" )
        
        if style.matrixStart is not None:
            print( style.matrixStart, end = "" )

        for r in range( len( m ) ):
            print( style.rowStart, end = "" )

            print( style.firstValue.format( m[r][0] ), end = "" )
            for c in range( 1, len( m[r] ) ):
                print( style.value.format(m[r][c]), end = "" )
            print( style.rowEnd, end="" )

        if style.matrixEnd is not None:
            print( style.matrixEnd, end="" )
        print()


def PrintMatrixInit( mat ):
    print( "LinearAlgebra::Matrix<float>( {{ ")
    for r in range( len( mat ) ):
        print( "{{ {}f".format( mat[r][0] ), end=' ' )
        for c in range( 1, len( mat[r] ) ):
            print( ", {}f".format( mat[r][c]), end=' ' )
        print( " }}, ")
    
    print( "}} ); ")

if __name__ == '__main__':
    m = GetMatrixSq( 6, -1.0, 1.0 )

    formatter = MatrixFormatter()
    formatter.PrintPrintMatrix( m, LinAlgCppMatrixStyle() )

    v = GetMatrix( 1, 6, -1.0, 1.0 )
    formatter.PrintPrintMatrix( v, LinAlgCppVectorStyle() )


    m = GetMatrixSq( 15, -1.0, 1.0 )
    formatter.PrintPrintMatrix( m, LinAlgCppMatrixStyle() )

    v = GetMatrix( 1, 15, -1.0, 1.0 )
    formatter.PrintPrintMatrix( v, LinAlgCppVectorStyle() )

    