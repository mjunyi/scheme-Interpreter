(define map (lambda (f l)
  (if (null? l)
      (quote ())
      (cons (f (car l)) (map f (cdr l))))))
(define not (lambda (f) (if f #f #t)))
        

