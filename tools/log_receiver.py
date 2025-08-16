#!/usr/bin/env python3
"""
Production-ready TCP log receiver with interactive UI
KISS principle - simple, reliable log collection
"""

import socket
import sys
import signal
import argparse
import threading
import queue
import time
from datetime import datetime

try:
    import curses
    CURSES_AVAILABLE = True
except ImportError:
    CURSES_AVAILABLE = False


class LogReceiver:
    def __init__(self, host='127.0.0.1', port=9000, interactive=False, log_file=None):
        self.host = host
        self.port = port
        self.socket = None
        self.running = True
        self.interactive = interactive
        self.log_file = log_file
        
        # Display toggles (interactive mode)
        self.show_timestamp = True
        self.show_ip = True
        self.show_port = True
        self.show_microseconds = True
        
        # For interactive mode
        self.log_queue = queue.Queue()
        self.stdscr = None
        
    def start(self):
        """Start the log receiver server"""
        if self.interactive and CURSES_AVAILABLE:
            self.start_interactive()
        else:
            self.start_simple()
    
    def start_simple(self):
        """Start in simple mode (original behavior)"""
        try:
            self.socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
            self.socket.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
            self.socket.bind((self.host, self.port))
            self.socket.listen(5)
            
            print(f"Log receiver listening on {self.host}:{self.port}")
            
            while self.running:
                try:
                    client_socket, client_addr = self.socket.accept()
                    print(f"Connection from {client_addr[0]}:{client_addr[1]}")
                    
                    # Handle client in thread for simple mode
                    client_thread = threading.Thread(
                        target=self.handle_client, 
                        args=(client_socket, client_addr)
                    )
                    client_thread.daemon = True
                    client_thread.start()
                    
                except socket.error as e:
                    if self.running:
                        print(f"Socket error: {e}")
                    break
                    
        except KeyboardInterrupt:
            print("\nShutdown requested")
        except Exception as e:
            print(f"Server error: {e}")
        finally:
            self.stop()
    
    def start_interactive(self):
        """Start in interactive curses mode"""
        try:
            curses.wrapper(self.interactive_main)
        except KeyboardInterrupt:
            pass
        finally:
            self.stop()
    
    def interactive_main(self, stdscr):
        """Main interactive interface using curses"""
        self.stdscr = stdscr
        curses.curs_set(0)  # Hide cursor
        stdscr.nodelay(1)   # Non-blocking input
        stdscr.timeout(100) # 100ms timeout for getch()
        
        # Start server in background thread
        server_thread = threading.Thread(target=self.run_server)
        server_thread.daemon = True
        server_thread.start()
        
        logs = []
        
        while self.running:
            # Handle keyboard input
            try:
                key = stdscr.getch()
                if key == ord('q'):
                    self.running = False
                    break
                elif key == ord('t'):
                    self.show_timestamp = not self.show_timestamp
                elif key == ord('i'):
                    self.show_ip = not self.show_ip
                elif key == ord('p'):
                    self.show_port = not self.show_port
                elif key == ord('m'):
                    self.show_microseconds = not self.show_microseconds
                elif key == ord('c'):
                    logs.clear()
            except:
                pass
            
            # Get new logs from queue
            try:
                while True:
                    log_entry = self.log_queue.get_nowait()
                    logs.append(log_entry)
                    if len(logs) > 1000:  # Keep last 1000 logs
                        logs.pop(0)
            except queue.Empty:
                pass
            
            # Draw interface
            self.draw_interface(stdscr, logs)
            time.sleep(0.1)
    
    def draw_interface(self, stdscr, logs):
        """Draw the interactive interface"""
        stdscr.clear()
        height, width = stdscr.getmaxyx()
        
        # Header
        header = f"Log Receiver - {self.host}:{self.port}"
        stdscr.addstr(0, 0, header[:width-1], curses.A_BOLD)
        
        # Controls
        controls = [
            "Controls: [q]uit | [t]imestamp | [i]p | [p]ort | [m]icroseconds | [c]lear",
            f"Show: Time:{self.show_timestamp} IP:{self.show_ip} Port:{self.show_port} μs:{self.show_microseconds}"
        ]
        
        for i, control in enumerate(controls):
            if i + 1 < height:
                stdscr.addstr(i + 1, 0, control[:width-1])
        
        # Separator
        if height > 3:
            stdscr.addstr(3, 0, "-" * (width-1))
        
        # Logs
        log_start = 4
        visible_logs = height - log_start - 1
        
        if logs and visible_logs > 0:
            start_idx = max(0, len(logs) - visible_logs)
            for i, log_entry in enumerate(logs[start_idx:]):
                y_pos = log_start + i
                if y_pos < height - 1:
                    display_text = self.format_log_entry(log_entry)
                    stdscr.addstr(y_pos, 0, display_text[:width-1])
        
        stdscr.refresh()
    
    def format_log_entry(self, log_entry):
        """Format a log entry based on current display settings"""
        timestamp, client_addr, message = log_entry
        
        parts = []
        
        if self.show_timestamp:
            if self.show_microseconds:
                time_str = timestamp
            else:
                time_str = timestamp.split('.')[0]
            parts.append(f"[{time_str}]")
        
        if self.show_ip or self.show_port:
            ip, port = client_addr
            if self.show_ip and self.show_port:
                parts.append(f"[{ip}:{port}]")
            elif self.show_ip:
                parts.append(f"[{ip}]")
            elif self.show_port:
                parts.append(f"[:{port}]")
        
        parts.append(message)
        return " ".join(parts)
    
    def run_server(self):
        """Run the socket server in background thread"""
        try:
            self.socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
            self.socket.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
            self.socket.bind((self.host, self.port))
            self.socket.listen(5)
            
            while self.running:
                try:
                    client_socket, client_addr = self.socket.accept()
                    
                    # Handle client in thread
                    client_thread = threading.Thread(
                        target=self.handle_client, 
                        args=(client_socket, client_addr)
                    )
                    client_thread.daemon = True
                    client_thread.start()
                    
                except socket.error:
                    if self.running:
                        break
        except Exception:
            pass
    
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
        
        # Full log entry (always saved if log_file specified)
        full_log = f"[{timestamp}] [{client_addr[0]}:{client_addr[1]}] {message}"
        
        # Save full log to file if specified
        if self.log_file:
            with open(self.log_file, 'a') as f:
                f.write(full_log + "\n")
        
        if self.interactive:
            # Add to queue for interactive display
            self.log_queue.put((timestamp, client_addr, message))
        else:
            # Simple mode - just print
            print(full_log)
    
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
    parser = argparse.ArgumentParser(description='TCP Log Receiver with Interactive UI')
    parser.add_argument('--host', default='127.0.0.1', help='Host to listen on')
    parser.add_argument('--port', type=int, default=9000, help='Port to listen on')
    parser.add_argument('--interactive', '-i', action='store_true', 
                       help='Enable interactive mode (requires curses)')
    parser.add_argument('--log-file', help='Save full logs to file')
    
    args = parser.parse_args()
    
    # Check if interactive mode is requested but curses not available
    if args.interactive and not CURSES_AVAILABLE:
        print("Interactive mode requires curses module. Install with: pip install windows-curses (Windows) or use system package manager")
        sys.exit(1)
    
    # Handle signals for clean shutdown
    signal.signal(signal.SIGINT, signal_handler)
    signal.signal(signal.SIGTERM, signal_handler)
    
    # Start receiver
    receiver = LogReceiver(args.host, args.port, args.interactive, args.log_file)
    
    if args.interactive:
        print("Starting interactive mode...")
        print("Controls: [q]uit | [t]imestamp | [i]p | [p]ort | [m]icroseconds | [c]lear")
        time.sleep(1)
    
    receiver.start()


if __name__ == '__main__':
    main()