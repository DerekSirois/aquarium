function rechauffe(){
    var xhttp = new XMLHttpRequest();
    xhttp.open("GET", "rechauffe", true);
    xhttp.send();
}
function pompe(){
    var xhttp = new XMLHttpRequest();
    xhttp.open("GET", "pompe", true);
    xhttp.send();
}
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