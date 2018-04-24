#pragma once
#include <vector>
#include "Names.h"

class EventManager {
private:
	std::vector<HANDLE> events;
public:
	HANDLE createEvent( std::wstring name, bool manualReset )
	{
		HANDLE event = CreateEventW(
			&sa,
			manualReset, false,
			name.c_str()
		);
		if( event == NULL ) {
			printf( "%d", GetLastError() );
			assert( false );
		}
		events.push_back( event );
		return event;
	}

	HANDLE openEvent( std::wstring name )
	{
		HANDLE event;
		event = OpenEventW( EVENT_ALL_ACCESS, false, name.c_str() );
		if( event == NULL ) {
			printf( "%d", GetLastError() );
			assert( false );
		}
		events.push_back( event );
		return event;
	}

	void setEvent( HANDLE event )
	{
		if( !SetEvent( event ) ) {
			printf( "%d", GetLastError() );
			assert( false );
		}
	}

	~EventManager()
	{
		for( auto& event : events ) {
			if( !CloseHandle( event ) ) {
				printf( "%d", GetLastError() );
				assert( false );
			}
		}
	}
};

class PartsEventManager : public EventManager {
public:
	void createEventsForWorkers( HANDLE* events, LPCWSTR name, bool manualReset, PROCESS_INFORMATION* procInfo )
	{
		for( int i = 0; i < N_PARTS; ++i ) {
			events[i] = createEvent( (std::wstring( name ) + std::to_wstring( procInfo[i].dwProcessId )), manualReset );
		}
	}

	void SetEventsForWorkers( HANDLE* events )
	{
		for( int i = 0; i < N_PARTS; ++i ) {
			setEvent( events[i] );
		}
	}


};
