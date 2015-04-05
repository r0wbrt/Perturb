include_directories("${PROJECT_SOURCE_DIR}/ExternalCode/Include")

set( THERON_LIB_SRC
	Theron/Actor.cpp 
	Theron/Address.cpp
	Theron/AllocatorManager.cpp
	Theron/BuildDescriptor.cpp 
	Theron/Clock.cpp 
	Theron/DefaultHandlerCollection.cpp 
	Theron/EndPoint.cpp 
	Theron/FallbackHandlerCollection.cpp 
	Theron/Framework.cpp 
	Theron/HandlerCollection.cpp 
	Theron/Receiver.cpp 
	Theron/StringPool.cpp 
	Theron/YieldPolicy.cpp)
	
add_library(Theron OBJECT ${THERON_LIB_SRC})

