import http.server
import socketserver
import subprocess
import json
import os
import tempfile

PORT = int(os.environ.get("PORT", 5000))
BASE_DIR = os.path.dirname(os.path.abspath(__file__))
COMPILER_BIN = os.path.join(BASE_DIR, "build", "switch_compiler")

class CompilerHandler(http.server.SimpleHTTPRequestHandler):
    def __init__(self, *args, **kwargs):
        super().__init__(*args, directory=os.path.join(BASE_DIR, "web"), **kwargs)

    def do_POST(self):
        if self.path == "/api/compile":
            content_length = int(self.headers.get("Content-Length", 0))
            post_data = self.rfile.read(content_length).decode("utf-8")
            
            try:
                payload = json.loads(post_data)
                code = payload.get("code", "")
            except Exception:
                code = post_data

            # Save to a temporary file
            with tempfile.NamedTemporaryFile(mode="w", suffix=".txt", delete=False) as tmp_file:
                tmp_file.write(code)
                tmp_path = tmp_file.name

            try:
                # Execute the exact switch_compiler binary in WSL / Linux
                proc = subprocess.run(
                    [COMPILER_BIN, tmp_path],
                    capture_output=True,
                    text=True,
                    timeout=5
                )

                response_data = {
                    "exit_code": proc.returncode,
                    "stdout": proc.stdout,
                    "stderr": proc.stderr,
                    "status": "SUCCESS" if proc.returncode == 0 else "ERROR"
                }
            except subprocess.TimeoutExpired:
                response_data = {
                    "exit_code": -1,
                    "stdout": "",
                    "stderr": "Compilation timed out.",
                    "status": "TIMEOUT"
                }
            except Exception as e:
                response_data = {
                    "exit_code": -1,
                    "stdout": "",
                    "stderr": str(e),
                    "status": "EXCEPTION"
                }
            finally:
                if os.path.exists(tmp_path):
                    os.remove(tmp_path)

            self.send_response(200)
            self.send_header("Content-Type", "application/json")
            self.send_header("Access-Control-Allow-Origin", "*")
            self.end_headers()
            self.wfile.write(json.dumps(response_data).encode("utf-8"))
        else:
            self.send_error(404, "Not Found")

    def do_OPTIONS(self):
        self.send_response(200)
        self.send_header("Access-Control-Allow-Origin", "*")
        self.send_header("Access-Control-Allow-Methods", "POST, GET, OPTIONS")
        self.send_header("Access-Control-Allow-Headers", "Content-Type")
        self.end_headers()

if __name__ == "__main__":
    # Ensure binary exists
    if not os.path.exists(COMPILER_BIN):
        print(f"Warning: {COMPILER_BIN} not found. Running make...")
        subprocess.run(["make"], cwd=BASE_DIR)

    socketserver.TCPServer.allow_reuse_address = True
    with socketserver.TCPServer(("", PORT), CompilerHandler) as httpd:
        print(f"==================================================")
        print(f" Switch-Case Mini Compiler Web UI running!")
        print(f" Open in your browser: http://localhost:{PORT}")
        print(f" Press Ctrl+C in terminal to stop.")
        print(f"==================================================")
        try:
            httpd.serve_forever()
        except KeyboardInterrupt:
            print("\nShutting down server.")
