### SynopFrame_Parser

You might need to change the 5th line in `SynopFrame_Parser/src/CMakeLists.txt` accordingly. For my case, it is the following: 

```
list( APPEND CMAKE_PREFIX_PATH "/Applications/Houdini/Houdini<19.0.657|Check_Your_Version>/Frameworks/Houdini.framework/Versions/Current/Resources/toolkit/cmake")
```


```
cd SynopFrame_Parser
cmake -S src -B build
cd build
make
```

If you see the following, it means `SynopFrame_Parser` is successfully installed. 

```
...
[100%] Linking CXX shared library /Users/luod/Library/Preferences/houdini/19.0/dso/SynopSpace_Parser.dylib
[100%] Built target SynopSpace_Parser
```
