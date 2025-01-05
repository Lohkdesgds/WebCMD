# WebCMD
*Just a quick project I made to have console tasks accessible on localhost http page*

<hr/>

### How to use it:

*Make a bat file following the format: (--port is optional)*

```bat
start "" <app> --port 8080 $ first_app.exe $ second_app.exe $ cmd /c "echo This should work and run as third"
exit
```

*This will create a localhost webpage with a table logging whatever first_app, second_app and that echo prints on a "virtual console"!*
*Mine is used with RClone. I don't like terminals hanging on my desktop. So using this, I have this:*

![image](https://github.com/Lohkdesgds/WebCMD/assets/17103500/fcbb88e9-6c6b-45d6-bb95-bb7458a705bc)

*As you can see, using Stop you can kill the app. Auto refresh is for auto "F5" the page for new stuff (you can see it reloading every second when ON).*
