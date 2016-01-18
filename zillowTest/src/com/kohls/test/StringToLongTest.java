package com.kohls.test;

import com.kohls.stringToLong.StringToLong;
import org.junit.Assert;
import org.junit.Test;

/**
 * Created by kohlsj on 1/17/16.
 */
public class StringToLongTest {

    @Test
    public void testProcessForRegularValue() {
        assertProcessResponse("12345", 12345l, null);
    }

    @Test
    public void testProcessForNegativeValue() {
        assertProcessResponse("-12345", -12345l, null);
    }

    @Test
    public void testProcessWithNoStringSupplied() {
        assertProcessResponse(null, null, "No String Supplied");
    }

    @Test
    public void testProcessWithNullStringSupplied() {
        assertProcessResponse("", null, "No String Supplied");
    }

    @Test
    public void testProcessWithNonNumberStringWithValueBelowZero() {
        assertProcessResponse("1235!24234234", null, "Non-number character supplied");
    }

    @Test

    public void testProcessWithNonNumberString() {
        assertProcessResponse("12B345", null, "Non-number character supplied");
    }

    @Test
    public void testProcessWithValueAboveMaxValue() {
        assertProcessResponse(String.valueOf(Long.MAX_VALUE) + "1", null, "Provided String overran max long value");
    }

    @Test
    public void testProcessWithValueBelowMinValue() {
        assertProcessResponse(String.valueOf(Long.MIN_VALUE) + "1", null, "Provided String overran max long value");
    }

    @Test
    public void testProcessWithOnlySignInString() {
        assertProcessResponse("-", null, "No Number past the indication of sign");
    }

    @Test
    public void testProcessWithLeadingCharacterBelowZeroButNotAccepted() {
        assertProcessResponse("!1234", null, "First character supplied not in reference of sign");
    }

    private void assertProcessResponse(String testValue, Long expectedValue, String message) {
        try {
            Long response = StringToLong.process(testValue);
            if (message != null) {
                Assert.fail("An exception should have been thrown");
            }
            Assert.assertEquals(expectedValue, response);
        } catch (Exception ex) {
            Assert.assertEquals(message, ex.getMessage());
        }
    }

}
