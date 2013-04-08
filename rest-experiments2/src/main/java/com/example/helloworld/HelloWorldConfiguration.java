package com.example.helloworld;

/**
 * Created with IntelliJ IDEA.
 * User: hmandala
 * Date: 4/8/13
 * Time: 2:59 PM
 * To change this template use File | Settings | File Templates.
 */
import com.yammer.dropwizard.config.Configuration;
import com.fasterxml.jackson.annotation.JsonProperty;
import org.hibernate.validator.constraints.NotEmpty;

public class HelloWorldConfiguration extends Configuration {
    @NotEmpty
    @JsonProperty
    private String template;

    @NotEmpty
    @JsonProperty
    private String defaultName = "Stranger";

    public String getTemplate() {
        return template;
    }

    public String getDefaultName() {
        return defaultName;
    }
}
