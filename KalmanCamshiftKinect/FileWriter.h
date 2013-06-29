/*
-----------------------------------------------------------------------------
Filename:    FileWriter.h
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
#ifndef __FileWriter_h_
#define __FileWriter_h_

#include "StdAfx.h"

#define WRITE   "w"
#define APPEND  "a"
#define READ    "r"  

/**
* Módulo de gestión de impresión a archivo de registro de actividades y tiempos en cada Experiencia.
*
* Ofrece una API sencilla para la inicialización y nombrado de archivos de registro, impresión de cabeceras y dumpeado de información mediante líneas de texto plano.
*
* @author Gabriel Sanmartín Díaz
*/
class FileWriter
{
public:
	FileWriter();

    FileWriter(std::string nombre);
    virtual ~FileWriter(void);
	//imprime al fichero la cabecera que describe los campos (solo la primera vez, si ya se ha impreso no se hace nada)
	void dumpHeader(Ogre::String header);
	//inicializa el puntero de archivo
	void init();
	//cierra el archivo
	void close();
	//imprime texto a fichero
	void dumpText(Ogre::String whatToDump);
	//imprime texto a fichero + NL
	void dumpTextNL(Ogre::String whatToDump);
	//imprime texto + timestamp
	void dumpDataTime(int elapMilli, Ogre::String whatToDump);
	//imprime un salto de línea
	void dumpNL();


protected:
	Ogre::String	fileName;
	bool			hdrDumped;
	FILE			*fp;
	bool			isOpen;

private:
	//genera el nombre del fichero con el nombre base + fecha y hora
	Ogre::String generaNombre(Ogre::String baseName);

};

#endif // #ifndef __FileWriter_h_
