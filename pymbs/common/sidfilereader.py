#-------------------------------------------------------------------------------
# Name:        Import SID-File
# Purpose:
#
# Author:      Martin
#
# Created:     26.10.2012
# Copyright:   (c) Martin 2012
# Licence:     <your licence>
#-------------------------------------------------------------------------------
import numpy as np


class SIDFormatException(Exception):
    '''
    '''

    def __init__(self, message):
        Exception.__init__(self, message)


class BaseFileReader(object):
    '''
    Stellt gemeinsame Funktionen zur Verfuegung
    '''

    def zeile_einlesen(self, f,n):

        # Zeile einlesen
        line = f.readline()

        # Nach Leerzeichen trennen
        content = line.split(None,n)
        return content

    def listenelement_check(self,liste, i):

        '''
        Pruefen ob ein Listenelement i existiert, wenn nicht dann
        Rueckgabe von 0
        '''
        try:
            return liste[i]
        except IndexError:
            return 0


    def listen_element_auslesen(self,content):

        matrix_element_betrag = content[1].split(None,1)
        matrix_element_betrag_as_string = matrix_element_betrag[0].replace('D', 'e')

        try:
                matrix_element_betrag_as_float = float(matrix_element_betrag_as_string)
        except ValueError:
                raise SIDFormatException("Falscher Datentyp, Float erwartet aber '%s' bekommen"%(matrix_element_betrag_as_string))
                # Python erkennt kein D fuer ^10 sondern nur e, Matrixwert

        content_l = content[0].split(None,0)

        if content_l[0].startswith('m1'):

            content_l = content_l[0].split('(')
            content_l = content_l[1].split(')')
            content_l = content_l[0].split(None,0)
            content_l = content_l[0].split(',')
            row_as_string = content_l[0].split(None,1)
            dim_as_string = content_l[1].split(None,1)
            col_as_string = content_l[2].split(None,1)

            try:
                row = int(row_as_string[0])
            except ValueError:
                raise SIDFormatException("Falscher Datentyp, int erwartet aber '%s' bekommen"%(row_as_string))

            try:
                dim = int(dim_as_string[0])
            except ValueError:
                raise SIDFormatException("Falscher Datentyp, int erwartet aber '%s' bekommen"%(dim_as_string))

            try:
                col = int(col_as_string[0])
            except ValueError:
                raise SIDFormatException("Falscher Datentyp, int erwartet aber '%s' bekommen"%(col_as_string))

            Ausgbe = [row,dim,col,matrix_element_betrag_as_float]

            return Ausgbe


        if content_l[0].startswith('m0'):

            content_l = content_l[0].split('(')
            content_l = content_l[1].split(')')
            content_l = content_l[0].split(None,0)
            content_l = content_l[0].split(',')
            row_as_string = content_l[0].split(None,1)
            col_as_string = content_l[1].split(None,1)

            try:
                row = int(row_as_string[0])
            except ValueError:
                raise SIDFormatException("Falscher Datentyp, int erwartet aber '%s' bekommen"%(row_as_string))


            try:
                col = int(col_as_string[0])
            except ValueError:
                raise SIDFormatException("Falscher Datentyp, int erwartet aber '%s' bekommen"%(col_as_string))

            Ausgbe = [row,col,matrix_element_betrag_as_float]

            return Ausgbe


class Matrix_built(BaseFileReader):

        '''
        Klasse zum Aufbau der entwickelten Taylor-Matrizen 0-ter,1-ter
        und n-ter Ordnung,
        desweiteren beinhaltet die Klasse eine Funktion um die Objekte
        wieder als String zu definieren
        '''

        def __init__(self, f):

            # Eigenschaften der Matrizen werden eingelesen
            # Zeile einlesen
            line = f.readline()
            content = line.split('=')
            content_l = content[0].split(None,0)

            if content_l[0].startswith('order'):
                content_r = content[1].split(None,1)

                # Ordnung der Taylorentwicklung
                try:
                    self.order = int(content_r[0])
                except ValueError:
                    raise SIDFormatException("Falscher Datentyp, Ints erwartet aber '%s' bekommen"%(content_r[0]))
            else:
                raise SIDFormatException("Keyword 'order' erwartet, aber '%s' bekommen"%(content_l[0] ))

            # Zeile einlesen
            line = f.readline()
            content = line.split('=')
            content_l = content[0].split(None,0)

            if content_l[0].startswith('nrow'):
                content_r = content[1].split(None,1)

                # Anzahl Zeilen
                try:
                    self.nrow = int(content_r[0])
                except ValueError:
                    raise SIDFormatException("Falscher Datentyp, Ints erwartet aber '%s' bekommen"%(content_r[0]))
            else:
                raise SIDFormatException("Keyword 'nrow' erwartet, aber '%s' bekommen"%(content_l[0] ))

            # Zeile einlesen
            line = f.readline()
            content = line.split('=')
            content_l = content[0].split(None,0)

            if content_l[0].startswith('ncol'):
                content_r = content[1].split(None,1)

                # Anzahl Spalten
                try:
                    self.ncol = int(content_r[0])
                except ValueError:
                    raise SIDFormatException("Falscher Datentyp, Ints erwartet aber '%s' bekommen"%(content_r[0]))
            else:
                raise SIDFormatException("Keyword 'ncol' erwartet, aber '%s' bekommen"%(content_l[0] ))

            # Zeile einlesen
            line = f.readline()
            content = line.split('=')
            content_l = content[0].split(None,0)

            if content_l[0].startswith('nq'):
                content_r = content[1].split(None,1)

                # Anzahl Moden
                try:
                    self.nq = int(content_r[0])
                except ValueError:
                    raise SIDFormatException("Falscher Datentyp, Ints erwartet aber '%s' bekommen"%(content_r[0]))
            else:
                raise SIDFormatException("Keyword 'nq' erwartet, aber '%s' bekommen"%(content_l[0] ))

            # Zeile einlesen
            # Zeile einlesen
            line = f.readline()
            content = line.split('=')
            content_l = content[0].split(None,0)

            if content_l[0].startswith('nqn'):
                content_r = content[1].split(None,1)

                # hoeherer Ordnung
                try:
                    self.nqn = int(content_r[0])
                except ValueError:
                    raise SIDFormatException("Falscher Datentyp, Ints erwartet aber '%s' bekommen"%(content_r[0]))
            else:
                raise SIDFormatException("Keyword 'nqn' erwartet, aber '%s' bekommen"%(content_l[0] ))

            # Zeile einlesen
            line = f.readline()
            content = line.split('=')
            content_l = content[0].split(None,0)

            if content_l[0].startswith('structure'):
                content_r = content[1].split(None,1)

                # Struktur der Matrix
                try:
                    self.structure = int(content_r[0])
                except ValueError:
                    raise SIDFormatException("Falscher Datentyp, Ints erwartet aber '%s' bekommen"%(content_r[0]))
            else:
                raise SIDFormatException("Keyword 'structure' erwartet, aber '%s' bekommen"%(content_l[0] ))

            # M0 und M1 wird als Nullmatrix erzeugt (M1 = Liste mit Null-
            # matrizen als Elemente)
            self.M0 = np.matrix(np.zeros([self.nrow,self.ncol]))

            self.M1 = list()
            m1 = np.matrix(np.zeros([self.nrow,self.ncol]))
            for i in range(self.nq):
                self.M1.append(m1.copy())

            # Zeile einlesen
            line = f.readline()
            content = line.split('=')
            content_l = content[0].split(None,0)


            # Aufbau der Matrix 0-ter Ordnung wenn keine 1-te Ordnung vorkommt
            if ((content_l[0].startswith('m0')) and (self.order == 0)):


                # Matrix auffuellen durch Schleife
                while not(content_l[0].startswith('end')):
                    # Zeileninhalt in Liste ablegen
                    matrixzeilen_inhalt = self.listen_element_auslesen(content)
                    # Liste auslesen und 1 subtrahieren, da Index beginnt bei
                    # Python bei 0
                    row = matrixzeilen_inhalt[0]-1
                    col = matrixzeilen_inhalt[1]-1
                    abs = matrixzeilen_inhalt[2]
                    # Wert an entsprechende Matrixstelle schreiben
                    self.M0[row,col] = abs
                    # zeile einlesen
                    line = f.readline()
                    content = line.split('=')
                    content_l = content[0].split(None,0)


            # Wenn self.order 1-ter Ordnung dann muessen 0-te und 1-te Ordnung
            # beruecksichtigt werden, es werden zuerst Nullmatrizen fuer 0-te
            # und 1-te Ordnung erzeugt
            if ((self.order == 1) and not(content_l[0].startswith('end'))):
                # Nullmatrix 0-ter Ordnung



                # Schleife zum befuellen der Matrizen 0-ter und  1-ter Ordnung
                while not(content_l[0].startswith('end')):
                    # Zeileninhalt in liste ablegen
                    matrixzeilen_inhalt = self.listen_element_auslesen(content)
                    # Matrix 0-ter Ordnung M0
                    if (len(matrixzeilen_inhalt) == 3):
                        # Liste auslesen und  1 subtrahieren, da Index bei 0
                        # beginnt
                        row = matrixzeilen_inhalt[0]-1
                        col = matrixzeilen_inhalt[1]-1
                        abs = matrixzeilen_inhalt[2]
                        # Matrix M0 befuellen
                        self.M0[row,col] = abs
                        # Zeile einlesen
                        line = f.readline()
                        content = line.split('=')
                        content_l = content[0].split(None,0)
                    # Matrix 1-ter Ordnung M1
                    if (len(matrixzeilen_inhalt) == 4):
                        # Liste auslesen und  1 subtrahieren, da Index bei 0
                        # beginnt
                        row = matrixzeilen_inhalt[0]-1
                        dim = matrixzeilen_inhalt[1]-1
                        col = matrixzeilen_inhalt[2]-1
                        abs = matrixzeilen_inhalt[3]
                        # Matrix M1 befuellen
                        self.M1[dim][row,col] = abs
                        #print self.M1
                        # zeile einlesen
                        line = f.readline()
                        content = line.split('=')
                        content_l = content[0].split(None,0)

            # String 'end' zeigt Ende der Matrix an
            if not(content_l[0].startswith('end')):
                raise SIDFormatException("Keyword 'end' erwartet, aber '%s' bekommen"%(content_l[0]))


        def M_ToString(self):
            '''
            Funktion wandelt die M0 und M1 Matrix in einen String um,
            im Falle einer Nullmatrix wird leere Liste zurueckgegeben
            '''
            '''
            M0 matrix wird zu erst in einen String umgewandelt
            '''
            # Zeilenindex
            i0 = 0
            # Spaltenindex
            j0 = 0
            # Liste wo von 0 verschiedene Werte der M0 Matrix abgelegt werden
            Werte0 = list()
            # Liste der dazugehoerigen Zeilen
            zeile0 = list()
            # Liste der dazugehoerigen Spalten
            spalte0 = list()
            # Iteration ueber jede Zeile

            for i0 in range(self.nrow):
                # Iteration ueber jede Spalte
                for j0 in range(self.ncol):
                    # Wenn Wert gefunden wird, der verschieden von 0 ist,
                    # dann wird Wert, Spalte und Zeile in entsprechender Liste
                    # abgelegt

                    if (self.M0[i0,j0] != 0):
                        zeile0.append(i0+1)
                        spalte0.append(j0+1)
                        Werte0.append(self.M0[i0,j0])

            # Zaehlindex fuer jeden Eintrag der Listen (Zeile, Spalte ...)
            h0 = 0
            # anlegen einer neuen Liste wo jeder Matrixwert als String mit ent-
            # sprechender Position verschluesslet wird und dann in der String-
            # liste abgelegt wird. Eine Stringliste beinhaltet immer die Matrix
            # 0-ter und/oder 1-ter Ordnung eines Objekts
            Stringliste0 = list()
            while h0 < len(zeile0):

                s0= '                          m0( %s, %s)                = %s\n'%(str(zeile0[h0]),str(spalte0[h0]),str(Werte0[h0]))
                Stringliste0.append(s0)
                h0 = h0+1

            # Stringliste wird zu einem String formatiert
            st0=''
            for k0 in Stringliste0:
                st0 = st0+k0

            '''
            wandelt die M1 matrix in einen String um,
            im Falle einer Nullmatrix wird leere Liste zurueckgegeben
            '''
            # siehe Kommentare bei Stringbildung von M0

            i = 0
            j = 0
            d = 0
            Werte = list()
            zeile = list()
            spalte = list()
            # Liste des Elements in M1 liste
            dim = list()

            # Itertion ueber jedes Element/Matrix
            for d in range(self.nq):
                for i in range(self.nrow):
                    for j in range(self.ncol):
                        if (self.M1[d][i,j] !=0):
                            dim.append(d+1)
                            zeile.append(i+1)
                            spalte.append(j+1)
                            Werte.append(self.M1[d][i,j])
            h = 0
            Stringliste = list()
            while h < len(zeile):
                s= '                          m1( %s, %s, %s)             = %s\n'%(str(zeile[h]),str(dim[h]),str(spalte[h]),str(Werte[h]))
                Stringliste.append(s)
                h = h+1

            st=''
            for k in Stringliste:
                st = st+k

            # Zusammenfuegen der Strings von M0 und M1
            Stringmatrix = st0+st


            if len(Stringmatrix) == 0:

                return Stringmatrix

            # Zeilenumbrauch muss entfernt werden, da sonst leere Zeile
            # geschrieben wird
            else:
                Stringmatrix = Stringmatrix[:-1]
                return Stringmatrix


        def ToString(self):
            '''
            Wandelt das Objekt bis structure in einen String um
            Matrizen muessen extra behandelt werden (siehe M0_ToString(),
            M1_ToString()
            '''
            s = ''
            s = '''                          order                    = %s
                          nrow                     = %s
                          ncol                     = %s
                          nq                       = %s
                          nqn                      = %s
                          structure                = %s \n'''% (str(self.order),str(self.nrow),str(self.ncol),str(self.nq),str(self.nqn),str(self.structure))
            return s


class De(BaseFileReader):
    '''
    Aufbau der Matrizen 0-ter, 1-ter und n-ter Ordnung nach der Taylor-Entw.
    '''

    def __init__(self, f):

        # Zeile einlesen
        content = self.zeile_einlesen(f,1)
        if (content[0] != 'De'):
            raise SIDFormatException("Keyword 'De' erwartet, aber '%s' bekommen"%(content[0] ))

        # Matrizen 0-ter und 1-ter Ordnung werden erzeugt und als Attribut
        # des Objekts gespeichert
        self.dematrix = Matrix_built(f)


    def ToString(self):
        '''
        Wandelt das Objekt in einen String um
        '''

        # aufbaumatrix ist String welcher die Struktur und den Aufbau der
        # jeweiligien Matrizen enthaelt
        aufbaumatrix = self.dematrix.ToString()
        # matrix ist String der die verschluesselten Matrizen enthaelt
        matrix = self.dematrix.M_ToString()
        # Zusammenfuegen beider Strings
        smatrix = aufbaumatrix+matrix

        if (len(matrix) == 0):
            # Zeilenumbrauch wird bei leeren String entfernt
            aufbaumatrix = aufbaumatrix[:-1]
            smatrix = aufbaumatrix



        s = ''
        s = '''           De
%s
           end De\n'''%(str(smatrix))
        return s

class Ke(BaseFileReader):
    '''
    Aufbau der Matrizen 0-ter, 1-ter und n-ter Ordnung nach der Taylor-Entw.
    '''

    # Kommentare siehe De-Klasse
    def __init__(self, f):

        # Zeile einlesen
        content = self.zeile_einlesen(f,1)
        if (content[0] != 'Ke'):
            raise SIDFormatException("Keyword 'Ke' erwartet, aber '%s' bekommen"%(content[0] ))

        self.kematrix = Matrix_built(f)


    def ToString(self):
        '''
        Wandelt das Objekt in einen String um
        '''
        aufbaumatrix = self.kematrix.ToString()
        matrix = self.kematrix.M_ToString()
        smatrix = aufbaumatrix+matrix

        if (len(matrix) == 0):
            aufbaumatrix = aufbaumatrix[:-1]
            smatrix = aufbaumatrix



        s = ''
        s = '''           Ke
%s
           end Ke\n'''%(str(smatrix))
        return s

class KSigma(BaseFileReader):
    '''
    Aufbau der Matrizen 0-ter, 1-ter und n-ter Ordnung nach der Taylor-Entw.
    '''

    # Kommentare siehe De-Klasse
    def __init__(self, f):

        # Zeile einlesen
        content = self.zeile_einlesen(f,1)
        if (content[0] != 'ksigma'):
            raise SIDFormatException("Keyword 'ksigma' erwartet, aber '%s' bekommen"%(content[0] ))

        self.ksigmamatrix = Matrix_built(f)



    def ToString(self):
        '''
        Wandelt das Objekt in einen String um
        '''
        aufbaumatrix = self.ksigmamatrix.ToString()
        matrix = self.ksigmamatrix.M_ToString()
        smatrix = aufbaumatrix+matrix

        if (len(matrix) == 0):
            aufbaumatrix = aufbaumatrix[:-1]
            smatrix = aufbaumatrix



        s = ''
        s = '''           ksigma
%s
           end ksigma\n'''%(str(smatrix))
        return s

class Oe(BaseFileReader):
    '''
    Aufbau der Matrizen 0-ter, 1-ter und n-ter Ordnung nach der Taylor-Entw.
    '''

    # Kommentare siehe De-Klasse
    def __init__(self, f):

        # Zeile einlesen
        content = self.zeile_einlesen(f,1)
        if (content[0] != 'Oe'):
            raise SIDFormatException("Keyword 'Oe' erwartet, aber '%s' bekommen"%(content[0] ))

        self.oematrix = Matrix_built(f)


    def ToString(self):
        '''
        Wandelt das Objekt in einen String um
        '''
        aufbaumatrix = self.oematrix.ToString()
        matrix = self.oematrix.M_ToString()
        smatrix = aufbaumatrix+matrix

        if (len(matrix) == 0):
            aufbaumatrix = aufbaumatrix[:-1]
            smatrix = aufbaumatrix


        s = ''
        s = '''           Oe
%s
           end Oe\n'''%(str(smatrix))
        return s



class Ge(BaseFileReader):
    '''
    Aufbau der Matrizen 0-ter, 1-ter und n-ter Ordnung nach der Taylor-Entw.
    '''

    # Kommentare siehe De-Klasse
    def __init__(self, f):

        # Zeile einlesen
        content = self.zeile_einlesen(f,1)
        if (content[0] != 'Ge'):
            raise SIDFormatException("Keyword 'Ge' erwartet, aber '%s' bekommen"%(content[0] ))

        self.gematrix = Matrix_built(f)


    def ToString(self):
        '''
        Wandelt das Objekt in einen String um
        '''
        aufbaumatrix = self.gematrix.ToString()
        matrix = self.gematrix.M_ToString()
        smatrix = aufbaumatrix+matrix

        if (len(matrix) == 0):
            aufbaumatrix = aufbaumatrix[:-1]
            smatrix = aufbaumatrix



        s = ''
        s = '''           Ge
%s
           end Ge\n'''%(str(smatrix))
        return s

class Gr(BaseFileReader):
    '''
    Aufbau der Matrizen 0-ter, 1-ter und n-ter Ordnung nach der Taylor-Entw.
    '''

    # Kommentare siehe De-Klasse
    def __init__(self, f):

        # Zeile einlesen
        content = self.zeile_einlesen(f,1)
        if (content[0] != 'Gr'):
            raise SIDFormatException("Keyword 'Gr' erwartet, aber '%s' bekommen"%(content[0] ))

        self.grmatrix = Matrix_built(f)


    def ToString(self):
        '''
        Wandelt das Objekt in einen String um
        '''
        aufbaumatrix = self.grmatrix.ToString()
        matrix = self.grmatrix.M_ToString()
        smatrix = aufbaumatrix+matrix

        if (len(matrix) == 0):
            aufbaumatrix = aufbaumatrix[:-1]
            smatrix = aufbaumatrix



        s = ''
        s = '''           Gr
%s
           end Gr\n'''%(str(smatrix))
        return s

class Me(BaseFileReader):
    '''
    Aufbau der Matrizen 0-ter, 1-ter und n-ter Ordnung nach der Taylor-Entw.
    '''

    # Kommentare siehe De-Klasse
    def __init__(self, f):

        # Zeile einlesen
        content = self.zeile_einlesen(f,1)
        if (content[0] != 'Me'):
            raise SIDFormatException("Keyword 'Me' erwartet, aber '%s' bekommen"%(content[0] ))

        self.mematrix = Matrix_built(f)


    def ToString(self):
        '''
        Wandelt das Objekt in einen String um
        '''
        aufbaumatrix = self.mematrix.ToString()
        matrix = self.mematrix.M_ToString()
        smatrix = aufbaumatrix+matrix

        if (len(matrix) == 0):
            aufbaumatrix = aufbaumatrix[:-1]
            smatrix = aufbaumatrix

        s = ''
        s = '''           Me
%s
           end Me\n'''%(str(smatrix))
        return s


class Cr(BaseFileReader):
    '''
    Aufbau der Matrizen 0-ter, 1-ter und n-ter Ordnung nach der Taylor-Entw.
    '''

    # Kommentare siehe De-Klasse
    def __init__(self, f):

        # Zeile einlesen
        content = self.zeile_einlesen(f,1)
        if (content[0] != 'Cr'):
            raise SIDFormatException("Keyword 'Cr' erwartet, aber '%s' bekommen"%(content[0] ))

        self.crmatrix = Matrix_built(f)


    def ToString(self):
        '''
        Wandelt das Objekt in einen String um
        '''
        aufbaumatrix = self.crmatrix.ToString()
        matrix = self.crmatrix.M_ToString()
        smatrix = aufbaumatrix+matrix

        if (len(matrix) == 0):
            aufbaumatrix = aufbaumatrix[:-1]
            smatrix = aufbaumatrix

        s = ''
        s = '''           Cr
%s
           end Cr\n'''%(str(smatrix))
        return s

class Ct(BaseFileReader):
    '''
    Aufbau der Matrizen 0-ter, 1-ter und n-ter Ordnung nach der Taylor-Entw.
    '''

    # Kommentare siehe De-Klasse
    def __init__(self, f):

        # Zeile einlesen
        content = self.zeile_einlesen(f,1)
        if (content[0] != 'Ct'):
            raise SIDFormatException("Keyword 'Ct' erwartet, aber '%s' bekommen"%(content[0] ))

        self.ctmatrix = Matrix_built(f)


    def ToString(self):
        '''
        Wandelt das Objekt in einen String um
        '''
        aufbaumatrix = self.ctmatrix.ToString()
        matrix = self.ctmatrix.M_ToString()
        smatrix = aufbaumatrix+matrix

        if (len(matrix) == 0):
            aufbaumatrix = aufbaumatrix[:-1]
            smatrix = aufbaumatrix

        s = ''
        s = '''           Ct
%s
           end Ct\n'''%(str(smatrix))
        return s

class J(BaseFileReader):
    '''
    Aufbau der Matrizen 0-ter, 1-ter und n-ter Ordnung nach der Taylor-Entw.
    '''

    # Kommentare siehe De-Klasse
    def __init__(self, f):

        # Zeile einlesen
        content = self.zeile_einlesen(f,1)
        if (content[0] != 'J'):
            raise SIDFormatException("Keyword 'J' erwartet, aber '%s' bekommen"%(content[0] ))

        self.Jmatrix = Matrix_built(f)


    def ToString(self):
        '''
        Wandelt das Objekt in einen String um
        '''
        aufbaumatrix = self.Jmatrix.ToString()
        matrix = self.Jmatrix.M_ToString()
        smatrix = aufbaumatrix+matrix

        if (len(matrix) == 0):
            aufbaumatrix = aufbaumatrix[:-1]
            smatrix = aufbaumatrix

        s = ''
        s = '''           J
%s
           end J\n'''%(str(smatrix))
        return s

class MDCM(BaseFileReader):
    '''
    Aufbau der Matrizen 0-ter, 1-ter und n-ter Ordnung nach der Taylor-Entw.
    '''

    # Kommentare siehe De-Klasse
    def __init__(self, f):

        # Zeile einlesen
        content = self.zeile_einlesen(f,1)
        if (content[0] != 'mdCM'):
            raise SIDFormatException("Keyword 'mdCM' erwartet, aber '%s' bekommen"%(content[0] ))

        self.mdcmmatrix = Matrix_built(f)


    def ToString(self):
        '''
        Wandelt das Objekt in einen String um
        '''
        aufbaumatrix = self.mdcmmatrix.ToString()
        matrix = self.mdcmmatrix.M_ToString()
        smatrix = aufbaumatrix+matrix

        if (len(matrix) == 0):
            aufbaumatrix = aufbaumatrix[:-1]
            smatrix = aufbaumatrix

        s = ''
        s = '''           mdCM
%s
           end mdCM\n'''%(str(smatrix))
        return s

class AP(BaseFileReader):
    '''
    Orientierung von e^(k,i) bezueglich e^(i) gemaess (6.79)
    '''

    # Kommentare siehe De-Klasse
    def __init__(self, f):

        # Zeile einlesen
        content = self.zeile_einlesen(f,1)
        if (content[0] != 'AP'):
            raise SIDFormatException("Keyword 'AP' erwartet, aber '%s' bekommen"%(content[0] ))

        self.apmatrix = Matrix_built(f)



    def ToString(self):
        '''
        Wandelt das Objekt in einen String um
        '''
        aufbaumatrix = self.apmatrix.ToString()
        matrix = self.apmatrix.M_ToString()
        smatrix = aufbaumatrix+matrix

        if (len(matrix) == 0):
            aufbaumatrix = aufbaumatrix[:-1]
            smatrix = aufbaumatrix

        s = ''
        s = '''                     AP
%s
                     end AP'''%(str(smatrix))
        return s

class Psi(BaseFileReader):
    '''
    Matrizen Psi^(k,i) aus (6.81) und (6.337)
    '''

    # Kommentare siehe De-Klasse
    def __init__(self, f):

        # Zeile einlesen
        content = self.zeile_einlesen(f,1)
        if (content[0] != 'psi'):
            raise SIDFormatException("Keyword 'psi' erwartet, aber '%s' bekommen"%(content[0] ))

        self.psimatrix = Matrix_built(f)


    def ToString(self):
        '''
        Wandelt das Objekt in einen String um
        '''
        aufbaumatrix = self.psimatrix.ToString()
        matrix = self.psimatrix.M_ToString()
        smatrix = aufbaumatrix+matrix

        if (len(matrix) == 0):
            aufbaumatrix = aufbaumatrix[:-1]
            smatrix = aufbaumatrix


        s = ''
        s = '''                     psi
%s
                     end psi'''%(str(smatrix))
        return s

class Phi(BaseFileReader):
    '''
    Matrizen Phi^(k,i) aus (6.80) und (6.337)
    '''

    # Kommentare siehe De-Klasse
    def __init__(self, f):

        # Zeile einlesen
        content = self.zeile_einlesen(f,1)
        if (content[0] != 'phi'):
            raise SIDFormatException("Keyword 'phi' erwartet, aber '%s' bekommen"%(content[0] ))

        self.phimatrix = Matrix_built(f)


    def ToString(self):
        '''
        Wandelt das Objekt in einen String um
        '''
        aufbaumatrix = self.phimatrix.ToString()
        matrix = self.phimatrix.M_ToString()
        smatrix = aufbaumatrix+matrix

        if (len(matrix) == 0):
            aufbaumatrix = aufbaumatrix[:-1]
            smatrix = aufbaumatrix


        s = ''
        s = '''                     phi
%s
                     end phi'''%(str(smatrix))
        return s

class Origin(BaseFileReader):
    '''
    Ort von O^(k,i) bezueglich O^(i) gemaess (6.78)
    Aufbau der Matrizen 0-ter, 1-ter und n-ter Ordnung nach der Taylor-Entw.
    '''

    # Kommentare siehe De-Klasse
    def __init__(self, f):

        # Zeile einlesen
        content = self.zeile_einlesen(f,1)
        if (content[0] != 'origin'):
            raise SIDFormatException("Keyword 'origin' erwartet, aber '%s' bekommen"%(content[0] ))

        self.originmatrix = Matrix_built(f)


    def ToString(self):
        '''
        Wandelt das Objekt in einen String um
        '''

        aufbaumatrix = self.originmatrix.ToString()
        matrix = self.originmatrix.M_ToString()
        smatrix = aufbaumatrix+matrix

        if (len(matrix) == 0):
            aufbaumatrix = aufbaumatrix[:-1]
            smatrix = aufbaumatrix

        s = ''
        s = '''                     origin
%s
                     end origin'''%(str(smatrix))
        return s

class Rframe(BaseFileReader):

    '''
    Koerperbezugssystem
    '''

    def __init__(self, f):

        #Zeile einlesen
        line = f.readline()
        content = line.split('=')
        content_l = content[0].split(None,0)

        if content_l[0].startswith('rframe'):
            content_r = content[1].split(None,3)

            # Bei Zeilenumbruch wird auch nur erster Name zuruekgegeben
            if (len(content_r)) == 1:

                self.refframe = content_r[0]

            else:

                self.refframe = content_r[0]+content_r[1]



        else:
            raise SIDFormatException("Keyword 'rframe' erwartet, aber '%s' bekommen"%(content_l[0] ))

        # Pruefen ob Listenelement 3 existiert,
        # wenn nein, wird 0 zurueckgegeben


    def ToString(self):
        '''
        Wandelt das Objekt in einen String um
        '''


        s = ''
        s = '                     rframe                        = %s'% (str(self.refframe))
        return s


class Node(BaseFileReader):

    '''
    Daten zur Berechnung der Bewegung von {O^(k,i),e^(k,i) an den Knoten k
    auf dem Koerper i und der Spannungen sigma^(k,i) aus (6.413)}
    '''
    nodenumber = 1
    def __init__(self, f,nNodes):

        #Zeile einlesen
        line = f.readline()
        content = line.split('=')
        content_l = content[0].split(None,0)
        content_l_l = content_l[0].split('node')
        content_l_l = content_l_l[0].split(None,1)
        content_l_r = content_l[0].split('new')
        content_l_r = content_l_r[1].split(None,1)

        if content_l_l[0] == 'new' and content_l_r[0] == 'node':
            content_r = content[1].split(None,1)
            # Knotennummer
            # int(content_r[0])

            self.node_number = Node.nodenumber
            #except ValueError:
             #   raise SIDFormatException("Falscher Datentyp, Ints erwartet aber '%s' bekommen"%(content_r[0]))
            # muesste mal noch verbessert werden
            # Notwendig f?r den Fall, dass Knotennummern durch Reduktion nicht von 1 beginnen
            if Node.nodenumber == nNodes:
                Node.nodenumber = 1
            else:
                Node.nodenumber = Node.nodenumber + 1
            # Neues rFrame Obejekt erzeugen
            self.rframe = Rframe(f)
            # Neues origin Objekt erzeugen
            self.origin = Origin(f)
            # Neues Phi Objekt erzeugen
            self.phi = Phi(f)
            # Neues Psi Objekt erzeugen
            self.psi = Psi(f)
            # Neues AP Objekt erzeugen
            self.ap = AP(f)


        else:
            raise SIDFormatException("Keyword 'new node' erwartet, aber '%s' %s' bekommen"%(content_l_l[0],content_l_r[0]))


        # Zeile einlesen
        # Zeile einlesen
        line = f.readline()
        pre_content = line.split(None,0)
        if pre_content[0].startswith('end'):
            content = pre_content[0].split('end')
            content_r = content[1].split(None,1)

        if (content_r[0] != 'node'):
            raise SIDFormatException("Keyword 'end node' erwartet, aber 'end %s' bekommen"%(content_r[0]))



    def ToString(self):
        '''
        Wandelt das Objekt in einen String um
        '''
        # Aufruf der ToString() Methode des Objekts rframe
        srframe = self.rframe.ToString()
        sorigin = self.origin.ToString()
        sphi = self.phi.ToString()
        spsi = self.psi.ToString()
        sap = self.ap.ToString()
        # Zusammenbau des Strings aus allen Objekten
        s = ''
        s = '''                new node                           = %s
%s
%s
%s
%s
%s
                end node\n'''% (str(self.node_number),str(srframe),str(sorigin),str(sphi),str(spsi),str(sap))
        return s



class Frame(BaseFileReader):
    '''
    Daten zur Berechnung der Bewegung von {O^(k,i),e^(k,i) an den Knoten k
    auf dem Koerper i und der Spannungen sigma^(k,i) aus (6.413)}
    '''

    def __init__(self, f,nNodes):

        # Naechste Zeile einlesen und nach Leerzeichen splitten
        content=self.zeile_einlesen(f,1)

        if (content[0] != 'frame'):
            raise SIDFormatException("Unerwartetes Dateiformat")

        # Anzahl der Knoten
        anz_Knoten = nNodes
        # Liste wo  Knotenobjekte abgelegt werden
        self.Knoten = list()
        # fuer jeden Knoten wird ein Knotenobjekt erstellt und in Liste
        # abgespeichert
        for i in range(anz_Knoten):
            node = Node(f,nNodes)
            self.Knoten.append(node)

        # Zeile einlesen
        line = f.readline()
        pre_content = line.split(None,0)
        if pre_content[0].startswith('end'):
            content = pre_content[0].split('end')
            content_r = content[1].split(None,1)

        if (content_r[0] != 'frame'):
            raise SIDFormatException("Keyword 'end frame' erwartet, aber 'end %s' bekommen"%(content_r[0]))






class Eigenmode(BaseFileReader):
    '''
    Beinhaltet die Eigenmoden, Eigenfrequenzen
    '''

    def __init__(self, f):

        line = f.readline()

        # Nach Gleich trennen
        content = line.split('=')
        content_l = content[0].split(None,0)
        test_flag = content_l[0].startswith('ielastq')

        if test_flag:
            content_l[0] = 'ielastq'

        else:
            raise SIDFormatException("Unerwartetes Dateiformat")

        content_r = content[1].split(None,0)
        test_flag = content_r[0].startswith('Eigen Mode')
        test_flag_1 = content_r[0].startswith('EigenMode')
        test_flag_2 = content_r[0].startswith('Eigenmode')
        test_flag_3 = content_r[0].startswith('Eigen mode')
        test_flag_4 = content_r[0].startswith('eigen mode')
        test_flag_5 = content_r[0].startswith('eigen Mode')
        test_flag_6 = content_r[0].startswith('eigenmode')
        test_flag_7 = content_r[0].startswith('eigenMode')

        if test_flag or test_flag_1 or test_flag_2 or test_flag_3 or test_flag_4 or test_flag_5 or test_flag_6 or test_flag_7:

            content_r = content_r[0].split(':')
            content_r_l = content_r[0].split(None,0)
            content_r_l = content_r_l[0].split('ode')
            content_number = content_r_l[1].split(None,1)
            content_r_r = content_r[1].split('Hz')
            content_r_r_freq = content_r_r[0].split(None,1)

            # Eigenschwingung
            try:
                self.eigenschwingung = int(content_number[0])
            except ValueError:
                raise SIDFormatException("Falscher Datentyp, Ints erwartet aber '%s' bekommen"%(content_number[0]))
            # Eigenfrequenz
            try:
                self.eigenfrequenz = float(content_r_r_freq[0])
                pass
            except ValueError:
                raise SIDFormatException("Falscher Datentyp, Float erwartet aber '%s' bekommen"%(content_r_r_freq[0]))



    def ToString(self,i):
        '''
        Wandelt das Objekt in einen String um
        '''

        s = ''
        s = '                ielastq (   %s)                     = Eigen Mode    %s : %s Hz\n'% (str(i),str(self.eigenschwingung),str(self.eigenfrequenz))
        return s



    def __repr__(self):
        '''
        Anzeige
        '''
        return "Eigenmode %f mit Frequenz %f"%(self.eigenschwingung, self.eigenfrequenz)

class Refmod(BaseFileReader):
    '''
    Beinhaltet die Masse, die Anzahl der generalisierten Koordinaten,
    Namen zur Identifikation
    '''

    def __init__(self, f):

        content=self.zeile_einlesen(f,1)

        # Pruefen der Laenge der Liste
        if (len(content) != 1):
            raise SIDFormatException("Unerwartetes Dateiformat")
        # Pruefen nach richtigen Keyword
        if (content[0] != 'refmod'):
            raise SIDFormatException("Keyword 'refmod' erwartet, aber '%s' bekommen"%content[0])

        # Masse
        line = f.readline()

        # Nach Doppelpunkt trennen
        content = line.split('=')
        content_l = content[0].split(None,1)

        if (content_l[0] != 'mass'):
            raise SIDFormatException("Unerwartetes Dateiformat")

        # Die Zahlen sind als 1.234D+003 geschrieben, Python versteht aber nur 1.234e+003
        content_r = content[1].split(None,1)
        content_r[0]=content_r[0].replace('D', 'e')
        self.mass = float(content_r[0])

        line = f.readline()

        # Nach Gleich trennen
        content = line.split('=')
        content_l = content[0].split(None,1)

        # Anzahl der generalisierten Koordinaten
        if (content_l[0] != 'nelastq'):
            raise SIDFormatException("Unerwartetes Dateiformat")

        content_r = content[1].split(None,1)

        try:
            self.nelastq = int(content_r[0])
        except ValueError:
            raise SIDFormatException("Falscher Datentyp, Ints erwartet aber '%s' bekommen"%(content_r[0]))

        # Fuer jede Eigenschwingung/Frequenz wird ein Objekt erstellt und in einer Liste ielastq abgelegt
        self.ielastq = list()
        for i in range(self.nelastq):
            self.eigenmode = Eigenmode(f)
            self.ielastq.append(self.eigenmode)

        # Zeile einlesen
        line = f.readline()
        pre_content = line.split(None,0)
        if pre_content[0].startswith('end'):
            content = pre_content[0].split('end')
            content_r = content[1].split(None,1)

        if (content_r[0] != 'refmod'):
            raise SIDFormatException("Keyword 'end refmod' erwartet, aber 'end %s' bekommen"%(content_r[0]))
##        self.ielastq = [Eigenmode(f) for i in xrange(self.nelastq)]


    def ToString1(self):
        '''
        Wandelt das Objekt in einen String um
        '''

        s = ''
        s = '''           refmod\n                mass                               = %s
                nelastq                            = %s\n'''% (str(self.mass),str(self.nelastq))
        return s




class Modal(BaseFileReader):
    '''
    Die Objekte der Klasse modal gehoeren ihrerseits zu den Klassen refmod, node, taylor
    '''

    def __init__(self, f, nNodes):

        line = f.readline()

        # Nach Doppelpunkt trennen
        content = line.split('=')
        content_l = content[0].split(None,2)
        content_r = content[1].split(None,1)

        # Pruefen nach richtigen Keyword


        if (len(content_l) > 2):
            raise SIDFormatException("Unerwartetes Dateiformat")
        if ((content_l[0] != 'new') and (content_l[1] != 'modal')):
            raise SIDFormatException("Keyword 'new modal' erwartet, aber '%s %s' bekommen"%(content_l[0], content_l[1]))

        if len(content_r) == 0:
            self.name = 'no_name_definied'

        else:
        # Name des Modals
            self.name = content_r[0]

        # neues refmod Objekt wird erstellt
        self.refmod = Refmod(f)
        # neues frame Objekt wird erstellt
        self.frame = Frame(f,nNodes)
        # neues mdCM Objekt wird erstellt
        self.mdcm = MDCM(f)
        # neues J Objekt wird erstellt
        self.j = J(f)
        # neues Ct Objekt wird erstellt
        self.ct = Ct(f)
        # neues Cr Objekt wird erstellt
        self.cr = Cr(f)
        # neues Me Objekt wird erstellt
        self.me = Me(f)
        # neues Gr Objekt wird erstellt
        self.gr = Gr(f)
        # neues Ge Objekt wird erstellt
        self.ge = Ge(f)
        # neues Oe Objekt wird erstellt
        self.oe = Oe(f)
        # neues ksigma Objekt wird erstellt
        self.ksigma = KSigma(f)
        # neues Ke Objekt wird erstellt
        self.ke = Ke(f)
        # neues De Objekt wird erstellt
        self.de = De(f)

        # Zeile einlesen
        # Zeile einlesen
        line = f.readline()
        pre_content = line.split(None,0)
        if pre_content[0].startswith('end'):
            content = pre_content[0].split('end')
            content_r = content[1].split(None,1)

        if (content_r[0] != 'modal'):
            raise SIDFormatException("Keyword 'end modal' erwartet, aber 'end %s' bekommen"%(content_r[0]))


        # Letzte Zeile einlesen
        # Zeile einlesen
        line = f.readline()
        pre_content = line.split(None,0)
        if pre_content[0].startswith('end'):
            content = pre_content[0].split('end')
            content_r = content[1].split(None,1)

        if (content_r[0] != 'part'):
            raise SIDFormatException("Keyword 'end part' erwartet, aber 'end %s' bekommen"%(content_r[0]))

    def ToString(self):
        '''
        Wandelt das Objekt in einen String um
        '''

        s = ''
        s = '      new modal                                    = %s    \n'% (str(self.name))
        return s




class SID(BaseFileReader):
    '''
    Ist Klasse des Hauptobjektes in dem alle anderen Objekte von Klassen
    erzeugt werden
    '''

    def __init__(self, f):
        '''
        enthaelt die Attribute eines Objektes
        '''

        line = f.readline()

        # Nach Doppelpunkt trennen
        content = line.split(':')
        content = content[0].split(None,2)


        #content=self.zeile_einlesen(f,2)

        # An Leerzeichen trennen, wird nach dem 2 Leerzeichen als ganzer String erkannt


        # Fehlerpruefung
        #assert len(content) >= 2, "Unerwartetes Dateiformat"
        if (len(content) < 2):
            raise SIDFormatException("Unerwartetes Dateiformat")

        try:
            # Die erste Zahl ist Anzahl der Nodes
            self.nNodes = int(content[0])
            # Die zweite Zahl ist Anzahl der Modes
            self.nModes = int(content[1])
        except ValueError:
            raise SIDFormatException("Falscher Datentyp, Ints erwartet aber '%s' und '%s' bekommen"%(content[0], content[1]))

        # Naechste Zeile einlesen
        content=self.zeile_einlesen(f,1)

        if (len(content)!=1):
            raise SIDFormatException("Unerwartetes Dateiformat")
        if (content[0] != 'part'):
            raise SIDFormatException("Keyword 'part' erwartet, aber '%s' bekommen"%content[0])



        # Neues Modal Objekt erzeugen
        self.modal = Modal(f, self.nNodes)


    def ToString(self):
        '''
        Wandelt das Objekt in einen String um
        '''

        s = ''
        s = '    %s   %s     :No. nodes, No. Modes\n part\n'% (str(self.nNodes), str(self.nModes))
        return s

'''''''''''''''''''''''''''''''''''''''''''''''
                Main Programm
'''''''''''''''''''''''''''''''''''''''''''''''
if __name__ == '__main__':

    '''
    Vorhandene Datei wird geoeffnet, Zeilen werden nacheinander eingelesen,
    die entsprechenden Objekte werden erstellt
    '''

    f = open("C:/Users/Martin/Desktop/SHK/Beispiel_SID_Files/Beispiel3EV.SID_FEM", "r")
    try:
        # sid Objekt wird erzeugt
        sid = SID(f)
    except SIDFormatException as fe:
        print("Datei konnte nicht eingelesen werden: " + fe.message)
    except:
        print("Unbekannter Fehler!")
    finally:
        # Datei wird geschlossen
        f.close()


##def ToFile(fw,str):
##    '''
##    Funktion zum Schreiben eines Strings in eine Datei
##    '''
##    fw.write(str)
##
##
##
##'''
##Neue leere Datei wird geoeffnet.
##Informationen die in den Objekten enthalten sind werden als String
##formuliert und mit der Funktion ToFile() in Datei geschrieben
##'''
##fw = open("C:/Users/Martin/Desktop/SHK/Beispiel_SID_Files/kleinertest.SID_FEM", 'w')
##
##s = sid.ToString()
##ToFile(fw,s)
##
##s = sid.modal.ToString()
##ToFile(fw,s)
##
##s = sid.modal.refmod.ToString1()
##ToFile(fw,s)
##
### Eigenfreuquenzen und die Eigenschwingungen werden aus der Liste ielastq
### ausgelesen, l ist die Nummerierung siehe SID File Beispiel
##l=1
##for i in sid.modal.refmod.ielastq:
##    s = i.ToString(l)
##    ToFile(fw,s)
##    l=l+1
##
##ToFile(fw,'           end refmod\n')
##
##ToFile(fw,'           frame\n')
##
### Fuer jeden Knoten werden die darin enthaltenen Objekte aufgerufen, als String
### formuliert und in Datei geschrieben.
##for i in sid.modal.frame.Knoten:
##    s = i.ToString()
##    ToFile(fw,s)
##
##ToFile(fw,'           end frame\n')
##
##s = sid.modal.mdcm.ToString()
##ToFile(fw,s)
##
##s = sid.modal.j.ToString()
##ToFile(fw,s)
##
##s = sid.modal.ct.ToString()
##ToFile(fw,s)
##
##s = sid.modal.cr.ToString()
##ToFile(fw,s)
##
##s = sid.modal.me.ToString()
##ToFile(fw,s)
##
##s = sid.modal.gr.ToString()
##ToFile(fw,s)
##
##s = sid.modal.ge.ToString()
##ToFile(fw,s)
##
##s = sid.modal.oe.ToString()
##ToFile(fw,s)
##
##s = sid.modal.ksigma.ToString()
##ToFile(fw,s)
##
##s = sid.modal.ke.ToString()
##ToFile(fw,s)
##
##s = sid.modal.de.ToString()
##ToFile(fw,s)
##
##ToFile(fw,'      end modal\n')
##ToFile(fw,' end part')
##fw.close()
