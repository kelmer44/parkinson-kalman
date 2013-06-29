#ifndef J_RECIBIR_H
#define J_RECIBIR_H

#include <winsock.h>

////////////////////////////////////////////////////////////
// Receiver.h
//
// Class definition for the recipient of a broadcasted message
//

#if !defined(WIN32) || defined(__CYGWIN__)
    #include <netinet/in.h>
#endif

namespace sockets{

    class J_Recibir
    {
        public :

	        J_Recibir();
	        ~J_Recibir();

   	        // setBuffer defines the buffer into which the broadcasted
	        // message will be received.
	        void setBuffer( void *buffer, const unsigned int size );

 	        // Define what port to listen and bind to
	        void setPort( const short port );
			inline const short getPort(){return _port;};
	        // Sync does a blocking wait to recieve next message
	        int sync( void );

        private :
	        bool init( void );

        private :
#if defined (WIN32) && !defined(__CYGWIN__)
            SOCKET _so;
            SOCKADDR_IN saddr;
#else
            int _so;
            struct sockaddr_in saddr;
#endif
            bool _initialized;
            short _port;
            void *_buffer;
            unsigned int _buffer_size;
    };
}
#endif 
