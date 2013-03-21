require "rubygems" 
require "selenium-webdriver"

Given /^that I have gone to the Google page$/ do
  @browser = Selenium::WebDriver.for :firefox
  @browser.navigate.to "http://google.com"
end 

When /^I add "(.*)" to the search box$/ do |item| 
  @element = @browser.find_element(:name, 'q')
  @element.send_keys item
end

When /^I clicked the Search Button$/ do
  @element.submit
end 

Then /^"(.*)" should be mentioned in the results$/ do |item| 
  @browser.find_element(:id=>"body").text.include? item
end

