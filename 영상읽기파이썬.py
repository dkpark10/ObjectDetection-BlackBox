# 파이썬에서 openCV를 사용하기 위해
import cv2

# 파일이 저장된 경로를 문자열 객체 지정 ./는 현재 디렉토리
imageFile = './Lenna.png' 

# RGB포맷으로 읽어서 img 객체에 저장한다.
img = cv2.imread(imageFile) # cv2.IMREAD_COLOR

# GRAY포맷으로 읽어서 img2 객체에 저장한다.
img2 = cv2.imread(imageFile, 0) # cv2.IMREAD_GRAYSCALE

# Lena color라는 이름으로 Window가 생성된다.
cv2.imshow('Lena color',img)

# Lena grayscale이라는 이름으로 Window가 생성된다.
cv2.imshow('Lena grayscale',img2)

# 키보드 입력 대기 - 키를 누르면 모든 창이 닫힌다.
cv2.waitKey( )
cv2.destroyAllWindows( )
