function showModalWin() {
    
    var darkLayer = document.createElement('div'); // слой затемнения
    darkLayer.id = 'shadow'; // id чтобы подхватить стиль
    document.body.appendChild(darkLayer); // включаем затемнение
 
    var modalWin = document.getElementsByClassName('auth'); // находим наше "окно"  

    // getElementsByClassName возвращае коллекцию, поэтому стоит индекс[0]
    
    
    modalWin[0].style.display = "block"; // "включаем" его 
    
    var btnCloseShadow  = document.getElementsByClassName("auth-btn-close-shadow");
    
    darkLayer.onclick = function () {  // при клике на слой затемнения все исчезнет
        darkLayer.parentNode.removeChild(darkLayer); // удаляем затемнение
        modalWin[0].style.display = 'none'; // делаем окно невидимым
        return false;
                };
    
    btnCloseShadow[0].onclick = function () {  // при клике на слой затемнения все исчезнет
        darkLayer.parentNode.removeChild(darkLayer); // удаляем затемнение
        modalWin[0].style.display = 'none'; // делаем окно невидимым
        return false;
                };
    
            }
function showModalWinOrder() {
    
    var darkLayer = document.createElement('div'); // слой затемнения
    darkLayer.id = 'shadow'; // id чтобы подхватить стиль
    document.body.appendChild(darkLayer); // включаем затемнение
 
    var modalWin = document.getElementsByClassName('do_order'); // находим наше "окно"  

    // getElementsByClassName возвращае коллекцию, поэтому стоит индекс[0]
    
    
    modalWin[0].style.display = "block"; // "включаем" его 
    
    var btnCloseShadow  = document.getElementsByClassName("auth-btn-close-shadow");
    
    darkLayer.onclick = function () {  // при клике на слой затемнения все исчезнет
        darkLayer.parentNode.removeChild(darkLayer); // удаляем затемнение
        modalWin[0].style.display = 'none'; // делаем окно невидимым
        return false;
                };
    
    btnCloseShadow[0].onclick = function () {  // при клике на слой затемнения все исчезнет
        darkLayer.parentNode.removeChild(darkLayer); // удаляем затемнение
        modalWin[0].style.display = 'none'; // делаем окно невидимым
        return false;
                };
    
            }