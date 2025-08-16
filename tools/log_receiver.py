#!/usr/bin/env python3
"""
Production-ready TCP log receiver
KISS principle - simple, reliable log collection
"""

import socket
import sys
import signal
import argparse
from datetime import datetime


class LogReceiver:
    def __init__(self, host='127.0.0.1', port=9000):
        self.host = host
        self.port = port
        self.socket = None
        self.running = True
        
    def start(self):
        """Start the log receiver server"""
        try:
            # Create socket
            self.socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
            self.socket.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
            
            # Bind and listen
            self.socket.bind((self.host, self.port))
            self.socket.listen(5)
            
            print(f"Log receiver listening on {self.host}:{self.port}")
            
            while self.running:
                try:
                    # Accept connection
                    client_socket, client_addr = self.socket.accept()
                    print(f"Connection from {client_addr[0]}:{client_addr[1]}")
                    
                    # Handle client
                    self.handle_client(client_socket, client_addr)
                    
                except socket.error as e:
                    if self.running:  # Only print if not shutting down
                        print(f"Socket error: {e}")
                    break
                    
        except KeyboardInterrupt:
            print("\nShutdown requested")
        except Exception as e:
            print(f"Server error: {e}")
        finally:
            self.stop()
    
    def handle_client(self, client_socket, client_addr):
        """Handle incoming log messages from a client"""
        try:
            while self.running:
                # Receive data
                data = client_socket.recv(4096)
                if not data:
                    break
                    
                # Process each line
                message = data.decode('utf-8', errors='replace')
                for line in message.split('\n'):
                    line = line.strip()
                    if line:
                        self.process_log(line, client_addr)
                        
        except socket.error:
            pass  # Client disconnected
        except Exception as e:
            print(f"Client error: {e}")
        finally:
            try:
                client_socket.close()
                print(f"Disconnected from {client_addr[0]}:{client_addr[1]}")
            except:
                pass
    
    def process_log(self, message, client_addr):
        """Process a single log message"""
        timestamp = datetime.now().strftime("%H:%M:%S.%f")[:-3]
        client_info = f"{client_addr[0]}:{client_addr[1]}"
        
        # Simple, clean output
        print(f"[{timestamp}] [{client_info}] {message}")
        
        # Optional: write to file
        # with open('received_logs.txt', 'a') as f:
        #     f.write(f"[{timestamp}] [{client_info}] {message}\n")
    
    def stop(self):
        """Stop the log receiver"""
        self.running = False
        if self.socket:
            try:
                self.socket.close()
            except:
                pass
        print("Log receiver stopped")


def signal_handler(signum, frame):
    """Handle shutdown signals"""
    print(f"\nReceived signal {signum}")
    sys.exit(0)


def main():
    parser = argparse.ArgumentParser(description='TCP Log Receiver')
    parser.add_argument('--host', default='127.0.0.1', help='Host to listen on')
    parser.add_argument('--port', type=int, default=9000, help='Port to listen on')
    
    args = parser.parse_args()
    
    # Handle signals for clean shutdown
    signal.signal(signal.SIGINT, signal_handler)
    signal.signal(signal.SIGTERM, signal_handler)
    
    # Start receiver
    receiver = LogReceiver(args.host, args.port)
    receiver.start()


if __name__ == '__main__':
    main()