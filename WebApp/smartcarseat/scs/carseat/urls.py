from django.urls import path
from . import views
from django.contrib.auth import views as auth_views


urlpatterns = [
   path('sendjson/', views.send_json,name = 'send_json'),
   path('register/', views.registerPage, name = 'register'),
   path('logout/', views.logoutUser, name = 'logout'),
   path('login/', views.loginPage, name = 'login'),
   path('',views.home, name= 'home'),
   path('reset_password/',
        auth_views.PasswordResetView.as_view(template_name="carseat/password_reset.html"),
        name="reset_password"),

   path('reset_password_sent/',
        auth_views.PasswordResetDoneView.as_view(template_name="carseat/password_reset_sent.html"),
        name="password_reset_done"),

   path('reset/<uidb64>/<token>/',
        auth_views.PasswordResetConfirmView.as_view(template_name="carseat/password_reset_form.html"),
        name="password_reset_confirm"),

   path('reset_password_complete/',
        auth_views.PasswordResetCompleteView.as_view(template_name="carseat/password_reset_done.html"),
        name="password_reset_complete"),

]
