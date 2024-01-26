package web_radio;

import java.io.*;
import java.net.ServerSocket;
import java.net.Socket;
import java.util.ArrayList;


class Server {
    private ServerSocket server;
    private ArrayList<ServerThread> serverThreads = new ArrayList<>();

    Server(int port) throws IOException {
        server = new ServerSocket(port);
    }

    void start() {
        // Verbindung der Server
        Socket socket = null;
        long clientnum = -1;

        ServerThread thread;

        Runtime.getRuntime().addShutdownHook(new Thread(() -> {
            System.out.println("Server wird heruntergefahren.");
            for ( ServerThread server: serverThreads ) {
                try {
                    server.stopClient();
                } catch (IOException e) {
                    System.out.println("Senden des Stopsignals für Client fehlgeschlagen: " + e);
                }
            }
        }));

        while (true) {
            try {
                socket = server.accept();
            } catch (IOException e) {
                System.out.println("I/O error: " + e);
                return;
            }
            clientnum += 1;
            thread = new ServerThread(socket, clientnum);
            thread.start();
            serverThreads.add(thread);
        }
    }

    private class ServerThread extends Thread {

        private Socket socket;
        private long clientnum;
        private String audiofilepath = "media/testfile.wav";
        private String audioFormat = ".wav\n";

        private ServerThread(Socket socket, long clientnum) {
            this.socket = socket;
            this.clientnum = clientnum;
        }

        void stopClient() throws IOException {
            PrintWriter printout = new PrintWriter(socket.getOutputStream());
            printout.print("quit Client\n");
            printout.flush();
        }

        private void wait_for_continue(InputStream in) throws IOException {
            while (in.read() != 0) ;
        }

        private void send_clientnum(OutputStream out) throws IOException {
            DataOutputStream datout = new DataOutputStream(out);
            datout.writeLong(clientnum);
            datout.flush();
        }

        private void send_format(OutputStream out) {
            PrintWriter printout = new PrintWriter(out);
            printout.print(audioFormat);
            printout.flush();
        }

        private void send_file(OutputStream out, String filepath) throws IOException {
            // File
            File file = new File(filepath);
            if (!file.exists() || !file.isFile())
                throw new IllegalArgumentException("not a file: " + file);

            // Stream
            BufferedInputStream filein = new BufferedInputStream( new FileInputStream(file) );
            DataOutputStream datout = new DataOutputStream(out);
            BufferedOutputStream bufout = new BufferedOutputStream(out);

            // Senden
            byte[] contents;
            long fileLength = file.length();
            datout.writeLong(fileLength);
            datout.flush();
            long current = 0;

            while (current<fileLength) {
                int size = 1024;
                if (fileLength - current < size)
                    size = (int) (fileLength - current);
                current += size;

                contents = new byte[size];
                filein.read(contents, 0, size);

                bufout.write(contents);
            }
            bufout.flush();
        }

        public void run() {
            try {
                OutputStream out = socket.getOutputStream();
                InputStream in = socket.getInputStream();

                wait_for_continue(in);

                //Senden der Clientnummer
                System.out.println("Senden (Clientnummber) start an: Client" + clientnum + ".");
                send_clientnum(out);
                System.out.println("Senden (Clientnummber) beendetan: Client" + clientnum + ".");

                wait_for_continue(in);

                //Senden des Audioformats als String
                System.out.println("Senden (Audioformat) start an: Client" + clientnum + ".");
                send_format(out);
                System.out.println("Senden (Audioformat) beendet an: Client" + clientnum + ".");

                wait_for_continue(in);

                //Senden der Musikdatei
                System.out.println("Senden (Musikdatei) start an: Client" + clientnum + ".");
                send_file(out, audiofilepath);
                System.out.println("Senden (Musikdatei) abgeschlossen an: Client" + clientnum + ".");

                System.out.println("\n");
            } catch (IOException e) {
                e.printStackTrace();
            } catch (IllegalArgumentException e) {
                System.out.println("Datei nicht vorhanden.");

            }
        }

    }
}
