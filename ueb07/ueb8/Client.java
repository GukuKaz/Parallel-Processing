package server_client;

import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStream;
import java.io.InputStreamReader;
import java.io.OutputStream;
import java.io.PrintWriter;
import java.net.ServerSocket;
import java.net.Socket;
import java.net.UnknownHostException;

public class Client {

	public static void main(String[] args) {
		
		 try{
		     // Verbindung der Client
             Socket client = new Socket("localhost", 5555);
             System.out.println("Client gestartet!");

             //Strem
             OutputStream out = client.getOutputStream();
             PrintWriter writer = new PrintWriter(out);

             InputStream in = client.getInputStream();
             BufferedReader reader = new BufferedReader(new InputStreamReader(in));
             //------------------------------------------------------------------------- 
        
             writer.write("Hello World!\n");
             writer.flush();
             
             
             String s= null;
             
             if ((s= reader.readLine())!=null){
            	 System.out.println("Empfangen vom Server:" +s);
             } 

             reader.close();
             writer.close();
            
        }
        catch (UnknownHostException e){
            e.printStackTrace();
        }
        catch (IOException e){
            e.printStackTrace();
        }
	}

}
