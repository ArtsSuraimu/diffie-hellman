

/*
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
import java.net.*;
import java.io.*;
import java.util.*;
import java.util.regex.*;
import java.math.*;
import java.nio.charset.Charset;
import java.util.logging.Level;
import java.util.logging.Logger;
import sun.misc.JavaxSecurityAuthKerberosAccess;

public class JClient {

    private final static int MAXMSGLEN = 1024;
    private final static int AF_INET = 2;  	// IPv4
    private final static int AF_INET6 = 10;   	// IPv6

    private static void printUsage() {
        System.err.printf("Usage: ./jclient -t <server> -p <port> "
                + "-<4|6>\n\n");
        System.err.printf("       -t: DNS name of server\n");
        System.err.printf("       -p: Port the server is listeningon\n");
        System.err.printf("       -4: Use IPv4\n");
        System.err.printf("       -6: Use IPv6\n");
        System.exit(-1);
    }

    private static void communicate(Socket sock, String file) throws IOException {
        /*
         * ==================================================================== *
         * YOUR TASK (see instructions below)                                   *
         * Implement the protocol as described in the assignment.               *
         *                                                                      *
         * Hint:                                                                *
         * The class RC4 implements the cipher. Use its member functions to     *
         *    - initialize RC4 with the shared secret K,                        *
         *    - to encrypt a buffer.                                            *
         * ==================================================================== *
         */
        DataInputStream in = new DataInputStream(sock.getInputStream());
        DataOutputStream out = new DataOutputStream(sock.getOutputStream());

        //out.writeChars("Hello");
        byte[] send = "HELLO".getBytes("US-ASCII");
        out.write(send);
        
        String tmp = in.readLine();
        System.out.println(tmp);
        
        byte[] buffer = new byte[MAXMSGLEN];
        String[] txt = tmp.substring(6).split(",");
        System.out.println(txt.length);
        
        int c = 5;
        BigInteger g,p,C,S,K;
        g = new BigInteger(txt[0]);
        p = new BigInteger(txt[1]);
        C = g.pow(c).mod(p);
        send = C.toString().getBytes("US-ASCII");
        System.out.println(C.toString());
        out.write(send);
        
        int read = in.read(buffer);
        System.out.println("recieved "+read+" Bytes");
        tmp = new String(buffer, 0, read, "US-ASCII").trim();
        System.out.println(tmp);
        S = new BigInteger(tmp);
        
        K = (S.pow(c)).mod(p);
        System.out.println(K.toString()+"\n"+K.longValue());
        RC4 rc4 = new RC4(K.longValue());
        

        send = "EncOK".getBytes("US-ASCII");
        System.out.println("DEBUG: send " + send.length + " Bytes");
        send = rc4.crypt(send, send.length);
        out.write(send);

        read = in.read(buffer);
        rc4.crypt(buffer, 5);
        System.out.println(new String(buffer, 0, 5, "US-ASCII"));

        send = file.getBytes("US-ASCII");
        rc4.crypt(send, send.length);
        out.write(send);

        read = in.read(buffer);
        rc4.crypt(buffer, read);
        System.out.println(new String(buffer, 0, read, "US-ASCII"));
        rc4.reset();
        /*
         * ==================================================================== *
         */
    }

    public static void main(String[] args) {
        BufferedReader fin = null;
        try {
            int af = -1;
            int port = -1;
            String target = "";
            for (int i = 0; i < args.length; ++i) {
                if (args[i].equals("-t")) {
                    ++i;
                    target = args[i];
                } else if (args[i].equals("-p")) {
                    ++i;
                    port = Integer.decode(args[i]);
                } else if (args[i].equals("-4")) {
                    af = AF_INET;
                    ++i;
                } else if (args[i].equals("-6")) {
                    af = AF_INET6;
                    ++i;
                }
            }
            if (target.equals("") || af == -1 || port == -1
                    || args.length != 5) {
                printUsage();
            }
            /*
             * ==================================================================== *
             * YOUR TASK (see instructions below)                                   *
             * 1) Connect to the server and call communicate(sock)                  *
             * 2) Implement communicate(int sock) (see above)                       *
             * ==================================================================== *
             */
            InetAddress addr = null;
            try {
                if (af == AF_INET) {
                    addr = Inet4Address.getByName("farina.net.in.tum.de");
                } else {
                    addr = Inet6Address.getByName("farina.net.in.tum.de");
                }
            } catch (UnknownHostException ex) {
                System.err.println("Couldn't find target");
                System.exit(1);
            }
            Socket dest = null;
            fin = new BufferedReader(new InputStreamReader(new FileInputStream(target)));
            Stack<String> ascii = new Stack<>();
            while (fin.ready()) {
                ascii.add("team128, ."+fin.readLine());
            }
            while (!ascii.empty()) {
                try {
                    dest = new Socket(addr, port);
                } catch (IOException ex) {
                    System.err.println("Couldn't connect to host");
                    System.exit(1);
                }
                try {

                    communicate(dest, ascii.pop());
                } catch (IOException ex) {
                    Logger.getLogger(JClient.class.getName()).log(Level.SEVERE, null, ex);
                } finally {
                    try {
                        dest.close();
                    } catch (IOException ex) {
                        Logger.getLogger(JClient.class.getName()).log(Level.SEVERE, null, ex);
                    }
                }
            }
        } catch (IOException ex) {
            Logger.getLogger(JClient.class.getName()).log(Level.SEVERE, null, ex);
        } finally {
            try {
                fin.close();
            } catch (IOException ex) {
                Logger.getLogger(JClient.class.getName()).log(Level.SEVERE, null, ex);
            }
        }
    }
}
