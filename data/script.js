$(document).ready(function(){
    var xhttp = new XMLHttpRequest();
    xhttp.onreadystatechange = function(){
        if(this.readyState == 4 && this.status == 200){
            document.getElementById("tempMin").value = this.responseText;
        }
    };
    xhttp.open("GET", "getTempMin", true);
    xhttp.send();
    var xhttp = new XMLHttpRequest();
    xhttp.onreadystatechange = function(){
        if(this.readyState == 4 && this.status == 200){
            document.getElementById("tempMax").value = this.responseText;
        }
    };
    xhttp.open("GET", "getTempMax", true);
    xhttp.send();
    var xhttp = new XMLHttpRequest();
    xhttp.onreadystatechange = function(){
        if(this.readyState == 4 && this.status == 200){
            document.getElementById("tempsEntre").value = this.responseText;
        }
    };
    xhttp.open("GET", "getTempsEntre", true);
    xhttp.send();
    var xhttp = new XMLHttpRequest();
    var xhttp = new XMLHttpRequest();
    xhttp.onreadystatechange = function(){
        if(this.readyState == 4 && this.status == 200){
            document.getElementById("tempsFiltre").value = this.responseText;
        }
    };
    xhttp.open("GET", "getTempsFiltre", true);
    xhttp.send();
    $("#appliquer").click(function(){
        var valeur = $("#tempMin").val();
        $.post("setTempsMin",{
            tempMin: valeur
        });
        var valeur = $("#tempMax").val();
        $.post("setTempsMax",{
            tempMax: valeur
        });
    });
    $("#appliquerPompe").click(function(){
        var valeur = $("#tempsEntre").val();
        $.post("setTempsEntre",{
            tempsEntre: valeur
        });
        var valeur = $("#tempsFiltre").val();
        $.post("setTempsFiltre",{
            tempsFiltre: valeur
        });
    });
});

setInterval(function getData(){
    var xhttp = new XMLHttpRequest();
    xhttp.onreadystatechange = function(){
        if(this.readyState == 4 && this.status == 200){
            document.getElementById("degre").innerHTML = this.responseText;
        }
    };
    xhttp.open("GET", "lireDegre", true);
    xhttp.send();
},2000);