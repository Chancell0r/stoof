package com.kohls.stringToLong;

/**
 * Created by kohlsj on 1/17/16.
 * <p/>
 * Converts a String to a Long using base 10. String provided can be denoted as positive
 * or negative. String cannot overrun the maximum/minimum values that a long can be.
 * <p/>
 * limitations: If a string is provided and has
 * 1. If the string is in fact equal to Long.MIN_VALUE then it will fail
 * 1. if a string element is multiple bytes in length, it could be misrepresented as another value.
 * 2. The string can only be parsed as base 10, whereas some might want to parse base 2 etc. However it would be
 * easy to implement it going to a different base, but I did not find it necessary since we are only worried about the string
 * supplied and can thus only assume that the value incoming is base 10.
 */
public class StringToLong {

    private static final char ZERO_CHAR = '0';

    private static final int BASE = 10;

    /**
     * Extract long value from String as base 10.
     *
     * @param string
     * @return
     */
    public static long process(String string) {
        boolean negativeFlag = false;
        int startIndex = 0;
        //check for empty or missing stringToLong
        if (string == null || string.isEmpty()) {
            throw new NumberFormatException("No String Supplied");
        }

        //characters above '0' or equal to '0' don't include - or +, and -/+ are below on the ascii table
        if (string.charAt(0) < ZERO_CHAR) {
            //if first character is -/+ and nothing past it, it's not a number
            if (string.length() == 1) {
                throw new NumberFormatException("No Number past the indication of sign");
            }
            negativeFlag = isNegative(string);
            startIndex++;
        }

        return convertToLong(negativeFlag, startIndex, string);
    }

    /**
     * Determine if the value is positive or negative.
     *
     * @param string
     * @return
     */
    private static boolean isNegative(String string) {
        boolean negative = false;
        //check for correct leading character
        if (string.charAt(0) == '-') {
            negative = true;
        } else if (string.charAt(0) != '+') {
            throw new NumberFormatException("First character supplied not in reference of sign");
        }

        return negative;
    }

    /**
     * Converts a string to a long value.
     *
     * @param negativeFlag - determines if the value returned should be negative or positive
     * @param startIndex   - a value has been found to indicate -/+
     * @param numberString - string of numbers to be converted.
     * @return
     */
    private static long convertToLong(boolean negativeFlag, int startIndex, String numberString) {
        long value = 0l;
        int stringLength = numberString.length();
        for (int i = startIndex; i < stringLength; i++) {
            //grab last character from stringToLong provided
            char currentChar = numberString.charAt(stringLength - (i - startIndex + 1));
            if (currentChar > '9' || currentChar < ZERO_CHAR) {
                throw new NumberFormatException("Non-number character supplied");
            }

            //subtracting to make use of the difference between Long.MAX_VALUE and Long.MIN_VALUE
            value -= (currentChar - ZERO_CHAR) * Math.pow(BASE, i - startIndex);

            //since Long.MIN_VALUE is greater in value than Long.MAX_VALUE if the number is more negative than
            //Long.MAX_VALUE then it has gone past its bounds.
            if (value < -Long.MAX_VALUE) {
                throw new NumberFormatException("Provided String overran max long value");
            }
        }

        //flip sign if nonNegative.
        return value * (negativeFlag ? 1 : -1);
    }

}
