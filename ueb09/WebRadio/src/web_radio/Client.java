package web_radio;


import javax.sound.sampled.*;
import java.io.*;
import java.net.Socket;
import java.net.UnknownHostException;

class Client {

    private String hostaddress;
    private int port;

    private String filepath = "";
    private Socket server;

    Client(String hostaddress, int port) {
        this.hostaddress = hostaddress;
        this.port = port;
    }

    private long read_num(InputStream in) throws IOException {
        DataInputStream datin = new DataInputStream(in);
        long num = datin.readLong();
        return num;
    }

    private String read_format(InputStream in) throws IOException {
        BufferedReader reader = new BufferedReader(new InputStreamReader(in));
        String s = reader.readLine();
        return s;
    }

    private void read_save_file(InputStream in) throws IOException {
        // File
        File file = new File(filepath);

        // Stream
        BufferedInputStream bufin = new BufferedInputStream(in);
        DataInputStream datin = new DataInputStream(in);
        BufferedOutputStream fileout = new BufferedOutputStream( new FileOutputStream(file) );

        // Empfangen
        long current = 0;
        long fileLength = datin.readLong();
        byte[] contents;
        int size;

        while (current < fileLength) {
            size = 65536;
            if (fileLength - current < size) {
                size = (int) (fileLength - current);
            }
            current += size;

            contents = new byte[size];
            bufin.read(contents, 0, size);

            fileout.write(contents);
        }
        fileout.flush();
    }

    void start() {
        // Verbindung der Client
        try {
            server = new Socket(hostaddress, port);
        } catch (UnknownHostException e) {
            System.out.println("Host konnte nicht gefunden werden.");
            return;
        } catch (IOException e) {
            System.out.println("Es ist ein Fehler aufgetreten." + e);
            return;
        }
        System.out.println("Client gestartet !");

        //Stream
        InputStream in = null;
        OutputStream out = null;
        try {
            in = server.getInputStream();
            out = server.getOutputStream();
        } catch (IOException e) {
            System.out.println("I/O Error: " + e);
            return;
        }
        //-------------------------------------------------------------------------

        // Empfangen der Clientnummer
        long clientnum;
        try {
            out.write(0);
            clientnum = read_num(in);
        } catch (IOException e) {
            System.out.println("I/O Error: " + e);
            return;
        }

        // Empfangen des Audioformats
        System.out.println("Empfangen (Audioformat) starten.");
        String s = null;
        try {
            out.write(0);
            s = read_format(in);
        } catch (IOException e) {
            System.out.println("I/O Error: " + e);
            return;
        }
        if (s != null) {
            System.out.println("Empfangen (Audioformat) beendet.");
        } else {
            System.out.println("Empfang (Audioformat) fehlgeschlagen.");
            return;
        }

        System.out.println("Audioformat: " + s);
        if (s.equals(".wav"))
            System.out.println("Audioformat angenommen ! ");
        else {
            System.out.println("Audioformat abgelehnt ! ");
            return;
        }

        filepath = "tmp" + clientnum + s;

        // Empfang und speichern der Musikdatei
        System.out.println("Empfangen (Audiodatei) starten.");
        try {
            out.write(0);
            read_save_file(in);
        } catch (IOException e){
            System.out.println("I/O Error: " + e);
            return;
        }
        System.out.println("Empfangen (Audiodatei) beendet");

        try {
            AudioInputStream audIS = AudioSystem.getAudioInputStream(
                    new File(filepath).getAbsoluteFile());
            Clip clip = AudioSystem.getClip();
            clip.open(audIS);
            clip.loop(Clip.LOOP_CONTINUOUSLY);
            clip.start();
        } catch (UnsupportedAudioFileException e) {
            System.out.println("Audiodatei ist fehlerhaft.");
        } catch (IOException e){
            System.out.println("I/O Error: " + e);
            return;
        } catch (LineUnavailableException e) {
            System.out.println("Es ist ein Fehler aufgetreten: " + e);
            return;
        }

        System.out.println("Soundausgabe gestartet.");



        try {
            BufferedReader reader = new BufferedReader(new InputStreamReader(server.getInputStream()));
            while (true) {
                String content = reader.readLine();
                if (content.equals("quit\n")) {
                    break;
                } else if ( content.startsWith("Client") ) {
                    System.out.println(content);
                }
            }
        } catch (IOException e) {
            System.out.println("I/O Error: " + e);
            return;
        } catch (NullPointerException e) {
            System.out.println("Server wurde beendet.");
        }
        System.out.println("Soundausgabe beendet.");

        File file = new File(filepath);
        file.delete();

        try {
            server.close();
        } catch (IOException e){
            System.out.println("I/O Error: " + e);
            return;
        }
    }
}