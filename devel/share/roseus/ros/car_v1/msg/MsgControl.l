;; Auto-generated. Do not edit!


(when (boundp 'car_v1::MsgControl)
  (if (not (find-package "CAR_V1"))
    (make-package "CAR_V1"))
  (shadow 'MsgControl (find-package "CAR_V1")))
(unless (find-package "CAR_V1::MSGCONTROL")
  (make-package "CAR_V1::MSGCONTROL"))

(in-package "ROS")
;;//! \htmlinclude MsgControl.msg.html


(defclass car_v1::MsgControl
  :super ros::object
  :slots (_data ))

(defmethod car_v1::MsgControl
  (:init
   (&key
    ((:data __data) 0)
    )
   (send-super :init)
   (setq _data (round __data))
   self)
  (:data
   (&optional __data)
   (if __data (setq _data __data)) _data)
  (:serialization-length
   ()
   (+
    ;; int32 _data
    4
    ))
  (:serialize
   (&optional strm)
   (let ((s (if strm strm
              (make-string-output-stream (send self :serialization-length)))))
     ;; int32 _data
       (write-long _data s)
     ;;
     (if (null strm) (get-output-stream-string s))))
  (:deserialize
   (buf &optional (ptr- 0))
   ;; int32 _data
     (setq _data (sys::peek buf ptr- :integer)) (incf ptr- 4)
   ;;
   self)
  )

(setf (get car_v1::MsgControl :md5sum-) "da5909fbe378aeaf85e547e830cc1bb7")
(setf (get car_v1::MsgControl :datatype-) "car_v1/MsgControl")
(setf (get car_v1::MsgControl :definition-)
      "int32 data

")



(provide :car_v1/MsgControl "da5909fbe378aeaf85e547e830cc1bb7")


