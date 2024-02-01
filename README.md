# Measure_Latency


En conclusión, podemos afirmar que en modo hackbench o bonnie++ las latencias deberían
aumentar siempre y cuando estemos utilizando arquitecturas No-RT, sin embargo, en arquitecturas
RT, esto no es tan predecible. En sistemas No-RT es lógico que las latencias aumenten ya que
estamos saturando el planificador, el aumento entre Raspberry y Linux será distinto ya que son
sistemas operativos diferentes. Por este motivo, podemos apreciar la relación que guardan entre los
sistemas No-RT. Por otra parte, en sistemas RT todas las tareas se deben cumplir en tiempo, por
ello, su planificador de tareas se aseugurará de cumplir todas las tareas en un tiempo medio. Gracias
a esto, podemos ver como en la Raspberry RT las latencias a penas varían.
En las gráficas que vamos a ver a continuación podemos ver como se cumple esto. En un
planificador No-RT, habrá mucha diferencia entre los distintos escenarios, al contrario que en un
planificador RT, las latencias serán muy parecidas.
En la última gráfica podremos ver una comparativa de dos planificadores distintos RT y No-RT, se
puede apreciar perfectamente como las latencias en un sistema RT son muy parecidas, a diferencia
de un sistema No-RT que irá haciendo sus tareas progresivamente, alterando y variando las
latencias.
