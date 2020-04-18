import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
//import java.net.ServerSocket;
import java.net.Socket;
//import java.net.URLEncoder;

//import java.net.InetAddress;
//import java.net.UnknownHostException;
//import java.util.Arrays;

import java.io.UnsupportedEncodingException;
//import sun.misc.*;

import javax.crypto.Cipher;
import javax.crypto.spec.IvParameterSpec;
import javax.crypto.spec.SecretKeySpec;
import java.util.Arrays;

import java.util.Scanner;

public class client
{
    public static Socket socket = new Socket();
    // public static Socket socket;
	public static OutputStream outputStream;
	public static InputStream inputStream;
    private static final String host = "127.0.0.1";
	private static final int port = 2333;
	private static final String key = "zhuangyonghaotql"; // 128 bit key
    private static final String initVector = "0000000000000000"; // 16 bytes IV
	
    public static void PREP() throws IOException
    {
        System.out.println("开始连接！");
		socket = new Socket(host, port);
		System.out.println("连接成功！");
    }
    /**
	 * Transfer byte array to String.
     * In order to support Chinese, use GBK.
	 */
	public static String ByteArraytoString(byte[] valArr,int maxLen) {
		String result=null;
		int index = 0;
		while(index < valArr.length && index < maxLen) {
			if(valArr[index] == 0) {
				break;
			}
			index++;
		}
		byte[] temp = new byte[index];
		System.arraycopy(valArr, 0, temp, 0, index);
		try {
			result= new String(temp,"GBK");
		} catch (UnsupportedEncodingException e) {
			e.printStackTrace();
		}
		return result;
    }
    
    /**
	 * Transfer String to byte array.
     * In order to support Chinese, use GBK.
	 */
	public static byte[] StringToByteArray(String str){
		byte[] temp = null;
	    try {
			temp = str.getBytes("GBK");
		} catch (UnsupportedEncodingException e) {
			e.printStackTrace();
		}
		return temp;
	}
	/**
     * String to hex String.
     */
    public static String str2HexStr(String str) {
 
        char[] chars = "0123456789ABCDEF".toCharArray();
        StringBuilder sb = new StringBuilder("");
        byte[] bs = str.getBytes();
        int bit;
        for (int i = 0; i < bs.length; i++) {
            bit = (bs[i] & 0x0f0) >> 4;
            sb.append(chars[bit]);
            bit = bs[i] & 0x0f;
            sb.append(chars[bit]);
        }
        return sb.toString();
    }
 
    /**
     *
     * Hex String to byte array.
     */
 
    public static byte[] hexStr2Bytes(String hexStr) {
        System.out.println("in len :" + hexStr.length());
        String str = "0123456789ABCDEF";
        char[] hexs = hexStr.toCharArray();
        byte[] bytes = new byte[hexStr.length() / 2];
        int n;
        for (int i = 0; i < bytes.length; i++) {
            n = str.indexOf(hexs[2 * i]) * 16;
            n += str.indexOf(hexs[2 * i + 1]);
            bytes[i] = (byte) (n & 0xff);
        }
        System.out.println("out len :" + bytes.length);
        System.out.println("ddd" + Arrays.toString(bytes));
        return bytes;
    }
 
    /**
     * byte array to hex String.
     */
    public static String byte2HexStr(byte[] b) {
        String hs = "";
        String stmp = "";
        for (int n = 0; n < b.length; n++) {
            stmp = (Integer.toHexString(b[n] & 0XFF));
            if (stmp.length() == 1)
                hs = hs + "0" + stmp;
            else
                hs = hs + stmp;
        }
        return hs.toUpperCase();
    }
	public static String Encrypt(String key, String initVector, String value)
	{
		try {
			IvParameterSpec iv = new IvParameterSpec(initVector.getBytes("UTF-8"));
			SecretKeySpec skeySpec = new SecretKeySpec(key.getBytes("UTF-8"), "AES");
			
			Cipher cipher = Cipher.getInstance("AES/CBC/PKCS5PADDING");
			cipher.init(Cipher.ENCRYPT_MODE, skeySpec, iv);
			
			byte[] encrypted = cipher.doFinal(value.getBytes());
			return byte2HexStr(encrypted);
		} catch (Exception ex) {
			ex.printStackTrace();
		}
		return null;
	}
	public static String Decrypt(String key, String initVector, String encrypted)
	{
		try {
			IvParameterSpec iv = new IvParameterSpec(initVector.getBytes("UTF-8"));
			SecretKeySpec skeySpec = new SecretKeySpec(key.getBytes("UTF-8"), "AES");
			
			Cipher cipher = Cipher.getInstance("AES/CBC/PKCS5PADDING");
			cipher.init(Cipher.DECRYPT_MODE, skeySpec, iv);
			
			byte[] original = cipher.doFinal(hexStr2Bytes(encrypted));
			return new String(original);
		} catch(Exception ex) {
			ex.printStackTrace();
		}
		return null;
	}
    public static String SEND(String _message) throws IOException
    {
        //Send message to server
        outputStream = socket.getOutputStream();

        String message = "#*#";
        _message = Encrypt(key, initVector, _message);
        message = message + _message + "#?#";
        
        //outputStream.write(message.getBytes("UTF-8"));
        outputStream.write(StringToByteArray(message));
        System.out.println("message: "+message);
        outputStream.flush();
        //Stop sending data.
        //如果不加shutdownOutput，服务器端一直阻塞在read()方法中，导致客户端无法收到服务端回显的数据
        //shutdownOutput只关闭客户端向服务端的输出流，并不会关闭整个Socket的连接
        socket.shutdownOutput();
 
        //Receiving data from server.
        inputStream = socket.getInputStream();
        byte[] bytes = new byte[1024];
		int len = 0;
        StringBuffer sb = new StringBuffer();
        System.out.println("START RECEIVING!");
        while ((len = inputStream.read(bytes)) != -1)
        {
        	//sb.append(new String(bytes, 0, len, "UTF-8"));
            sb.append(ByteArraytoString(bytes, bytes.length));
            System.out.println(len);
            //System.out.println("客户端正在从输入流中读数据");
        }
        System.out.println("来自服务端的回显:" + sb.toString());

        //process the message received
        int p_st = sb.indexOf("#*#") + 3;
        int p_en = sb.indexOf("#?#");
        String _recv_m = sb.substring(p_st,p_en);
        //process _recv_m
        String _recv_message = Decrypt(key, initVector, _recv_m);
        //String _recv_message = _recv_m;

        return _recv_message;
    }

    public static void END_SOCKET() throws IOException
    {
        //调用close方法，会直接关闭整个Socket连接
        outputStream.close();
        inputStream.close();
        socket.close();
        System.out.println("END THIS SOCKET!");
    }

    public static void main(String[] args) throws IOException
    {
        String outmes = "FUCK YOU";
        String backmes = "";
        int i = 1;
        while (i<2000)
        {
            PREP();
            outmes = outmes + String.valueOf(i);


            @SuppressWarnings("resource")
			Scanner sc = new Scanner(System.in); outmes = sc.nextLine();


            System.out.println("outmes : "+outmes);
            backmes = SEND(outmes);
            System.out.println("backmes : "+backmes);
            ++i;
            END_SOCKET();
        }
    }
}
