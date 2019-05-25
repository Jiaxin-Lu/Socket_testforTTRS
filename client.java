package com.example.runzhe.ticket;

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

import java.util.Scanner;

public class client
{
    public static Socket socket = new Socket();
    // public static Socket socket;
	public static OutputStream outputStream;
	public static InputStream inputStream;
    private static final String host = "59.78.38.126";
	private static final int port = 2333;
	
    public static void PREP() throws IOException
    {
        System.out.println("开始连接！");
		socket = new Socket(host, port);
		System.out.println("连接成功！");
    }

    /*public static String getUTF8(String _str)
    {
        StringBuffer sb = new StringBuffer();
        sb.append(_str);
        String str = "";
        String strUTF8 = "";
        try{
            str = new String(sb.toString().getBytes("UTF-8"));
            strUTF8 = URLEncoder.encode(str,"UTF-8");
        } catch (UnsupportedEncodingException e) {
            e.printStackTrace();
        }
        return strUTF8;
    }*/
    /*public static String getGBK(String _str) throws UnsupportedEncodingException
    {
        byte[] bytes = _str.getBytes("UTF-8");
        return (new String(bytes, "GBK"));
    }
    public static String getUTF8(String _str) throws UnsupportedEncodingException
    {
    	return new String(_str.getBytes("UTF-8"),"UTF-8"); 
    }*/
    /**
	 * 将byte数组转化成String,为了支持中文，转化时用GBK编码方式
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
	 * 将String转化为byte,为了支持中文，转化时用GBK编码方式
	 */
	public static byte[] StringToByteArray(String str){
		byte[] temp = null;
	    try {
			temp = str.getBytes("GBK");
		} catch (UnsupportedEncodingException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
		return temp;
	}
    /*public static String FinalEncode(String _str)
    {
        BASE64Encoder encoder = new BASE64Encoder();
        byte[] b = _str.getBytes("UTF-8");
        String __str = encoder.encode(b);
        return __str;
    }

    public static String FinalDecode(String _str)
    {
        BASE64Decoder decoder = new BASE64Decoder();
        String __str = "";
        try {
        byte[] b = decoder.decodeBuffer(_str);
        __str = new String(b, "UTF-8");
        }
        catch (UnsupportedEncodingException e)
        {
        	
        }
        return __str;
    }*/

    public static String SEND(String _message) throws IOException
    {
        //向服务端发送数据
        outputStream = socket.getOutputStream();
        //修正_message
        //_message = getUTF8(_message);

        String message = "#*#";
        //int _message_len = _message.length();
        message = message + _message + "#?#";
        //byte[] sen_bytes = StringToByteArray(message);

        //message = FinalEncode(message); //ENCODE
        
        //outputStream.write(message.getBytes("UTF-8"));
        outputStream.write(StringToByteArray(message));
        System.out.println("message: "+message);
        outputStream.flush();
        //终止发送数据，客户端现在只接收数据
        //如果不加shutdownOutput，服务器端一直阻塞在read()方法中，导致客户端无法收到服务端回显的数据
        //shutdownOutput只关闭客户端向服务端的输出流，并不会关闭整个Socket的连接
        socket.shutdownOutput();
 
        //从服务端接收数据
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

        //process the sb received
        int p_st = sb.indexOf("#*#") + 3;
        //int p_en = sb.indexOf("#",p_st);
        //String len_message_ = sb.substring(p_st,p_en);
        //System.out.println("back len_message : "+len_message_);
        //int len_message = Integer.valueOf(len_message_);
        int p_en = sb.indexOf("#?#");
        String _recv_m = sb.substring(p_st,p_en);
        //process _recv_m
        //String _recv_message = FinalDecode(_recv_m); //decode
        String _recv_message = _recv_m;

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


            Scanner sc = new Scanner(System.in); outmes = sc.nextLine();


            System.out.println("outmes : "+outmes);
            backmes = SEND(outmes);
            System.out.println("backmes : "+backmes);
            ++i;
            END_SOCKET();
        }
    }
}
