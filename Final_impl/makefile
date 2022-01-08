travelMonitorClient: travelMonitorClient.cpp monitorServer.cpp hw/skip.h hw/country.h hw/bloom.h hw/person.h
	g++ travelMonitorClient.cpp hw/bloom.cpp -o travelMonitorClient -std=c++11 -pthread
	g++ monitorServer.cpp hw/skip.cpp hw/country.cpp hw/bloom.cpp hw/person.cpp -o monitorServer -std=c++11 -pthread
clean:
	rm travelMonitorClient
	rm monitorServer