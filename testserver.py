import socket
import threading
import sys


def receive_loop(conn, stop_event):
    try:
        while not stop_event.is_set():
            data = conn.recv(4096)
            if not data:
                print("Remote side closed the connection.")
                stop_event.set()
                break
            try:
                text = data.decode('utf-8', errors='replace')
            except Exception:
                text = repr(data)
            print(text, end='')
    except Exception as exc:
        print(f"Receive error: {exc}")
        stop_event.set()


def main():
    host = '0.0.0.0'
    port = 5001

    with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as server_sock:
        server_sock.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
        server_sock.bind((host, port))
        server_sock.listen(1)
        print(f"Listening on {host}:{port}...")

        conn, addr = server_sock.accept()
        stop_event = threading.Event()
        with conn:
            print(f"Connected by {addr}")
            receiver = threading.Thread(target=receive_loop, args=(conn, stop_event), daemon=True)
            receiver.start()

            try:
                while not stop_event.is_set():
                    try:
                        line = input()
                    except EOFError:
                        break
                    except KeyboardInterrupt:
                        print()
                        break

                    if stop_event.is_set():
                        break

                    try:
                        conn.sendall((line+"\n").encode('utf-8'))
                    except Exception as exc:
                        print(f"Send error: {exc}")
                        stop_event.set()
                        break
            finally:
                stop_event.set()
                print("Closing connection.")


if __name__ == '__main__':
    try:
        main()
    except KeyboardInterrupt:
        print("\nServer stopped.")
        sys.exit(0)
