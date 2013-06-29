/*
-----------------------------------------------------------------------------
Filename:    FileWriter.cpp
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
#include "StdAfx.h"
#include "FileWriter.h"


FileWriter::FileWriter(): fp(0), isOpen(false), hdrDumped(false)
{

}
//-------------------------------------------------------------------------------------
FileWriter::FileWriter(std::string nombre): hdrDumped(false)
{
	fileName = generaNombre(nombre);
}
//-------------------------------------------------------------------------------------
FileWriter::~FileWriter(void)
{
}


//Abrimos el fichero y lo dejamos abierto hasta que se cierre explícitamente
void FileWriter::init(){
	fp = fopen(fileName.c_str(), WRITE);
	if(fp)
		isOpen=true;
}

//Cerramos el fichero, ¡NO OLVIDAR!
void FileWriter::close(){
	if(fp!=NULL && isOpen){
		fclose(fp);
		isOpen=false;
	}
	
}

void FileWriter::dumpHeader(Ogre::String header){
	if(!hdrDumped && fp!=NULL){
		dumpText(header);
		dumpNL();
		hdrDumped = true;
	}
}


void FileWriter::dumpText(Ogre::String whatToDump){
	if(!fp){ 
		printf("Error al volcar datos a fichero."); 
		return; 
	}
	fprintf(fp, "%s", whatToDump.c_str());
}



void FileWriter::dumpTextNL(Ogre::String whatToDump){
	dumpText(whatToDump);
	dumpNL();
}


void FileWriter::dumpDataTime(int elapMilli, Ogre::String whatToDump){
	dumpText(whatToDump);
	dumpText(Ogre::StringConverter::toString(elapMilli));
	dumpNL();

}

void FileWriter::dumpNL(){  
	dumpText("\n");
}


Ogre::String FileWriter::generaNombre(Ogre::String baseName){
	Ogre::String nombreArchivo;
	nombreArchivo = Ogre::String(baseName);
	char *f = (char *)malloc(sizeof(char) * 12);
	Util::getGregorianDate(f);
	nombreArchivo.append(f);
	nombreArchivo.append("@");
	f = (char *)malloc(sizeof(char) * 9);
    Util::getCompleteTime(f);
	nombreArchivo.append(f);
	nombreArchivo.append(".txt");

 	return nombreArchivo;
}
