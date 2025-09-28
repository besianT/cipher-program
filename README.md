# cipher-program
A program which encrypts and decrypts text using Caesar ciphers and Substitution ciphers.
Upon running this program we will get a menu where we have several command options to choose from.

The command c will allow us to enter a text which will be encrypted using the Caesar cipher algorithm. What this does is takes our text and shifts each letter by a certain number. For example, if we shift by 2 "a" will become "c" , "b" will become "d" etc.

The command d will allow us to do the opposite, that is, take a Caesar encrypted string and decrypt it back to normal using the reverse algorthm described above.

The command a will allow us to apply a substitute cipher on a text/string, which is a stronger encryption than the caesar encryption. It works by mapping each letter in the alphabet to another randomly. For example "a" becomes "z", "z" becomes "c". It basicly scrambles each letter in the string.

The command e will compute how "english" our string appears, if we have a completly encrypted string, we will get a very low score, if something is decrypted, it will get a higher score.

The command s will decrypt our substitute encrypted text.

Finally, the command f will do the same as command s, but it will allow us to enter an entire file to decrypt.
