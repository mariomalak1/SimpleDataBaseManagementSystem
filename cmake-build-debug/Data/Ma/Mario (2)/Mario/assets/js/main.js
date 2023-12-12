/*=============== Validation ===============*/
function validateForm() {
    let username = document.getElementById("login-email").value;
    let password = document.getElementById("login-pass").value;

    if (username === "") {
        alert("يرجى إدخال اسم المستخدم.");
        return false;
    }

    if (password === "") {
        alert("يرجى إدخال كلمة المرور.");
        return false;
    }
    return true;
}

/*=============== Change Logo When Mobile Screen ===============*/
let image = document.querySelector(".image");

window.addEventListener("resize", () => {
    if (window.innerWidth <= 768) {
        image.setAttribute("src", "assets/images/logo-mobile.png");
    } else {
        image.setAttribute("src", "assets/images/الشعار.png");
    }
})