# Some Notes on the original Article

## The IF statement
The BNF shown on this chapter had some bugs. The author said that `<program>`
and `<block>` are as follows:

```
<program> ::= <block> END
<block> ::= [ <statement> ]*
```

That means a `<block>` can contain zero or more statments. However, there is
no way to decide when to terminate a `<block>`. When dealing with "IF" without
"ELSE", the problem is not yet obvious:

```
IF <condition> <block> ENDIF
```

Because the author imply that `<block>` should not be ended with token "END",
now we can treat token "ENDIF" which is represent by character 'e' as the
termination of block. That means source code like "aibced" is allowed, aka
"bc" this two statements are treated as one block.

When it comes to "IF" statement with "ELSE", things just change.

```
IF <condition> <block> [ ELSE <block>] ENDIF
```

While we are dealing with the first `<block>`, there is no way to quit
recognizing `<block>` and match token "ELSE". Especially when the author
propose test case "aiblcede". It was expected to recognize "b" as a single
block and "l" as token "ELSE". Which however is not possible because there is
no way to determine the exit of `<block>`.

```
procedure Block;
begin
   while not(Look in ['e']) do begin
      case Look of
       'i': DoIf;
       'o': Other;
      end;
   end;
end;
```

The source code implies that every block should end with token "END" which is
character 'e'. One way to fix this is to explicitly state that a `<block>`
ends with token "END". So the test case should be "aibelceede" and the BNF
should explicitly be:

```
<program> ::= <block>
<block> ::= [ <statement> ]* END
```
