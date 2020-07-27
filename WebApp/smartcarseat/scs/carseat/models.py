from django.db import models
# Create your models here.

class Customer(models.Model):
    temp = models.CharField(max_length=200 , null=True)
    lat = models.CharField(max_length=200, null= True)
    long = models.CharField(max_length=200, null= True)
    safe = models.CharField(max_length=9, null= True)

