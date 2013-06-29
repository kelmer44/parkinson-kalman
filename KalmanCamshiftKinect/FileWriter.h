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
* M�dulo de gesti�n de impresi�n a archivo de registro de actividades y tiempos en cada Experiencia.
*
* Ofrece una API sencilla para la inicializaci�n y nombrado de archivos de registro, impresi�n de cabeceras y dumpeado de informaci�n mediante l�neas de texto plano.
*
* @author Gabriel Sanmart�n D�az
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
	//imprime un salto de l�nea
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
