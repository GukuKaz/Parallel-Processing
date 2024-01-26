package web_radio;

import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStreamReader;

public class Main {

    public static void main(String[] args) {
        BufferedReader cin = null;

        cin = new BufferedReader( new InputStreamReader(System.in));
        char c;
        do {
            try {
                System.out.print("Soll dies ein Server oder ein Client werden: (S/C)\t");
                c = cin.readLine().toLowerCase().charAt(0);
            } catch (IOException e) {
                e.printStackTrace();
                return;
            }
        } while (c != 's' && c != 'c');

        if (c == 's') {
            System.out.println("Server wird erstellt!");
            int port;
            Server serv;
            do {
                try {
                    System.out.print("Welcher Port soll benutzt werden:\t");
                    port = Integer.parseInt(cin.readLine());
                    serv = new Server(port);
                    break;
                } catch (IOException e) {
                    System.out.println("Ihre Eingabe war kein gültiger Port.");
                }
            } while (true);
            serv.start();
        } else {
            System.out.println("Client wird erstellt!");
            String hostadress = null;
            int port;
            Client clie;
            do {
                try {
                    System.out.print("Mit welcher Adresse soll der Client sich verbinden?:\t");
                    hostadress = cin.readLine();
                    System.out.print("Mit welchem Port soll der Client sich verbinden?:\t");
                    port = Integer.parseInt(cin.readLine());
                    clie = new Client(hostadress, port);
                    break;
                } catch (IOException e) {
                    e.printStackTrace();
                }
            } while (true);
            clie.start();
        }
    }
}
