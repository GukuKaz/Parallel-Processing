package server_client;

import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStream;
import java.io.InputStreamReader;
import java.io.OutputStream;
import java.io.PrintWriter;
import java.net.ServerSocket;
import java.net.Socket;


public class Server {
	
	public static void main(String[] args) {
		
		 try { 
		        // Verbindung der Server
		        ServerSocket server = new ServerSocket(5555);
		        System.out.println("Server gestartet !");
		        
		        Socket client = server.accept();
		        
		        //Strem
		        OutputStream out = client.getOutputStream();
		        PrintWriter writer = new PrintWriter(out);

		        InputStream in = client.getInputStream();
		        BufferedReader reader = new BufferedReader(new InputStreamReader(in));
		        //-------------------------------------------------------------------------

		       
		        String s = null;
		    
		        //Ausgabe und zurückschreiben an Client
		        while((s= reader.readLine()) !=null){
		        	writer.write(s + "\n");
		        	writer.flush();
		            System.out.println("Empfangen vom Client:" +s);
		        } 
		        
		        writer.close();
		        reader.close();
		       
		        
		    }
		    catch(IOException e){
		    e.printStackTrace();
		    }
	}

}
