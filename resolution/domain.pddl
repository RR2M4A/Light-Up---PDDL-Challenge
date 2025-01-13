(define
    (domain domain-lightsup)
    (:requirements :strips :typing :negative-preconditions)
    (:types
        line col
    )
    
    (:predicates
        (is-on ?l - line ?c - col)
        (is-visible ?l1 - line ?c1 - col ?l2 - line ?c2 - col)
        (is-numbered-adjacent ?l1 - line ?c1 - col ?l2 - line ?c2 - col)
        (counter-at ?l - line ?c - col ?cnt - counter)
    )

    (:constants i0 i1 i2 i3 i4 - counter)
    
    (:action bulb
    
        :parameters(?l - line ?c - col)
        :precondition (not (is-on ?l ?c))
        
        :effect (and

            (is-on ?l ?c)
            
            (forall (?c2 - col) (and
                ; Ativando as células da mesma linha
                (when (or 
                        (is-visible ?l ?c ?l ?c2)
                        (is-visible ?l ?c2 ?l ?c)
                    )
                        (is-on ?l ?c2)
                )

                (when (is-numbered-adjacent ?l ?c ?l ?c2) (and
                    (when (counter-at ?l ?c2 i0) (and (not (counter-at ?l ?c2 i0)) (counter-at ?l ?c2 i1)))
                    (when (counter-at ?l ?c2 i1) (and (not (counter-at ?l ?c2 i1)) (counter-at ?l ?c2 i2)))
                    (when (counter-at ?l ?c2 i2) (and (not (counter-at ?l ?c2 i2)) (counter-at ?l ?c2 i3)))
                    (when (counter-at ?l ?c2 i3) (and (not (counter-at ?l ?c2 i3)) (counter-at ?l ?c2 i4)))
                ))

            ))
            
            (forall (?l2 - line) (and
                ; Ativando as células da mesma coluna
                (when (or 
                        (is-visible ?l ?c ?l2 ?c)
                        (is-visible ?l2 ?c ?l ?c)
                    )
                        (is-on ?l2 ?c)
                )

                (when (is-numbered-adjacent ?l ?c ?l2 ?c) (and
                    (when (counter-at ?l2 ?c i0) (and (not (counter-at ?l2 ?c i0)) (counter-at ?l2 ?c i1)))
                    (when (counter-at ?l2 ?c i1) (and (not (counter-at ?l2 ?c i1)) (counter-at ?l2 ?c i2)))
                    (when (counter-at ?l2 ?c i2) (and (not (counter-at ?l2 ?c i2)) (counter-at ?l2 ?c i3)))
                    (when (counter-at ?l2 ?c i3) (and (not (counter-at ?l2 ?c i3)) (counter-at ?l2 ?c i4)))
                ))
            ))
        )
    )
)