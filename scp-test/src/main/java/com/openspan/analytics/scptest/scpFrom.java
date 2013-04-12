package com.openspan.analytics.scptest;

/**
 * Created with IntelliJ IDEA.
 * User: hmandala
 * Date: 4/12/13
 * Time: 10:24 AM
 * To change this template use File | Settings | File Templates.
 */

import com.jcraft.jsch.*;
import java.io.*;

public class scpFrom
{
    public static void main(String args[])
    {

        try
        {
            JSch jsch = new JSch();
            jsch.addIdentity("key loc here");

            Session session = jsch.getSession("ubuntu", "url here", 22);
            System.out.println("session created.");

            java.util.Properties config = new java.util.Properties();
            config.put("StrictHostKeyChecking", "no");
            session.setConfig(config);
            session.connect();
            System.out.println("session connected.....");

            ChannelSftp channel = (ChannelSftp)session.openChannel("sftp");
            channel.connect();
            try
            {
                System.out.println("Trying to get file");
                InputStream is = channel.get("/home/ubuntu/Downloads/test.txt");
                System.out.println("Got file");
                System.out.println(getStringFromInputStream(is));
            }
            catch (com.jcraft.jsch.SftpException ex)
            {
            }

            //channel.setInputStream(System.in);
            //channel.setOutputStream(System.out);


            //channel.connect();


        }

        catch (com.jcraft.jsch.JSchException ex)
        {
            System.out.println(ex.toString());
        }


    }
    private static String getStringFromInputStream(InputStream is) {

        BufferedReader br = null;
        StringBuilder sb = new StringBuilder();

        String line;
        try {

            br = new BufferedReader(new InputStreamReader(is));
            while ((line = br.readLine()) != null) {
                sb.append(line);
            }

        } catch (IOException e) {
            e.printStackTrace();
        } finally {
            if (br != null) {
                try {
                    br.close();
                } catch (IOException e) {
                    e.printStackTrace();
                }
            }
        }

        return sb.toString();

    }

}
