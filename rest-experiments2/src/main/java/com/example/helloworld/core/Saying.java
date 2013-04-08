package com.example.helloworld.core;

/**
 * Created with IntelliJ IDEA.
 * User: hmandala
 * Date: 4/8/13
 * Time: 3:05 PM
 * To change this template use File | Settings | File Templates.
 */
public class Saying
{
    private final long id;
    private final String content;

    public Saying(long id, String content) {
        this.id = id;
        this.content = content;
    }

    public long getId() {
        return id;
    }

    public String getContent() {
        return content;
    }
}
