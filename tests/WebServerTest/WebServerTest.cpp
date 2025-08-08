// All includes must be at the top for proper test macro expansion
#include <gtest/gtest.h>
#include <WebServer/WebServer.h>
#include <IoCContainer/IoCContainer.h>
#include <memory>
#include <thread>
#include <chrono>
#include <string>
#include <fstream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
